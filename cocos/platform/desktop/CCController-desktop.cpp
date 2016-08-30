//
//  CCController-desktop.cpp
//  cocos2d_libs
//
//  Created by Chad Ata on 2/10/15.
//
//


#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#include "CCController-desktop.h"

#include "CCGLViewImpl-desktop.h"
#include "cocos2d.h"
#include <SDL.h>
static const float AXIS_MAX = 32767.0f;



NS_CC_BEGIN


void ControllerDesktop::setButtonMapping(const ButtonMapping* b)
{
	this->buttonMapping = b;
}

// TODO - controller - implement disconnect event
// TODO - controller - add destructor and shutdown operations
ControllerImpl::ControllerImpl()
{
}

ControllerImpl::~ControllerImpl()
{
	for (int i = 0; i < 2; ++i)
	{
		if (controllers[i])
		{
			SDL_GameControllerClose(controllers[i]);
			controllers[i] = NULL;
		}
	}
}

/**
 * supporting just 2 controllers for now
 */
void ControllerImpl::init()
{
	controllers[0] = NULL;
	controllers[1] = NULL;
	// TODO - cleanup 
	int controllerIndex = 0;
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			SDL_GameController *controller = SDL_GameControllerOpen(i);
			controllers[controllerIndex] = controller;
//			log("Opened Joystick %d on %d\n", i, controllerIndex);
//			log("Name: %s\n", SDL_GameControllerName(controller));
			if (++controllerIndex >= 2)
				break;
		}
	}
}

/**
 * Polls for controller events and dispatches the results
 */
void ControllerImpl::pollJoysticks()
{
	// polling approach
	SDL_Event event;	// TODO - optimize
	// TODO - move poll events call out of here in the future if more functionality is required
	while (SDL_PollEvent(&event))
	{
		// TODO - handle connect/disconnect
        switch( event.type )
        {
            case SDL_CONTROLLERDEVICEADDED:
            {
                // NOTE: event.cdevice.which is the device index in this case
                // could not find a reliable to id controller without opening it - so here goes with open/close
                SDL_GameController* controller = SDL_GameControllerOpen( event.cdevice.which );
                if( controller )
                {
                    bool isAssigned = false;
                    // check if already assigned
                    for( int i = 0; i < MAX_CONTROLLERS; ++i )
                    {
                        if( controllers[i] == controller )
                        {
                            isAssigned = true;
                            break;
                        }
                    }
                    if( !isAssigned )
                    {
                        // if not assigned and we have room to assign it
                        for( int i = 0; i < MAX_CONTROLLERS; ++i )
                        {
                            if( controllers[i] == NULL )
                            {
                                controllers[i] = controller;
                                isAssigned = true;
                                break;
                            }
                        }
                        // if no room to assign it, close it
                        if( !isAssigned )
                            SDL_GameControllerClose( controller );
                    }
                }
                break;
            }
            case SDL_CONTROLLERDEVICEREMOVED:
                // NOTE: event.cdevice.which is the instance id in this case
                for( int i = 0; i < MAX_CONTROLLERS; ++i )
                {
                    if( controllers[i]  )
                    {
                        SDL_bool isAttached = SDL_GameControllerGetAttached(controllers[i]);
                        if( !isAttached )
                        {
                            ControllerDesktop* controller = (ControllerDesktop*)Controller::getControllerByTag(i);
                            // zero out all of the axes - sometimes before disconnect the control spits out a bunch of junk axis data
                            if( controller )
                            {
                                controller->onAxisEvent(Controller::Key::JOYSTICK_LEFT_X, 0, true);
                                controller->onAxisEvent(Controller::Key::JOYSTICK_LEFT_Y, 0, true);
                                controller->onAxisEvent(Controller::Key::JOYSTICK_RIGHT_X, 0, true);
                                controller->onAxisEvent(Controller::Key::JOYSTICK_RIGHT_Y, 0, true);
                            }
                            controllers[i] = NULL;
                        }
                    }
                }
                break;
            default:
                break;
        }
	}

	if (controllers[0])
		pollJoystick(0, controllers[0]);
	if (controllers[1])
		pollJoystick(1, controllers[1]);
}

void ControllerImpl::pollJoystick(int id, SDL_GameController* sdlController)
{
	if (!SDL_GameControllerGetAttached(sdlController))
		return;

	ControllerDesktop* controller = (ControllerDesktop*)Controller::getControllerByTag(id);
	const ButtonMapping* buttonMapping = &buttonMappingXBoxOne;		// TODO - deprecate button mapping since we're using sdl controller now?
	if (controller == NULL)
	{
		const char* name = SDL_GameControllerName(sdlController);
		// It's a new controller being connected.
		controller = new cocos2d::ControllerDesktop();
		controller->_deviceId = id;
		controller->_deviceName = name;
		controller->setTag(id);
		controller->isSharedTriggerAxis = false;
		controller->setButtonMapping(buttonMapping);
		Controller::s_allController.push_back(controller);
		controller->onConnected();
	}
	buttonMapping = controller->buttonMapping;

	Uint8 buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_A);
	controller->onButtonEvent(Controller::Key::BUTTON_A, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_B);
	controller->onButtonEvent(Controller::Key::BUTTON_B, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_X);
	controller->onButtonEvent(Controller::Key::BUTTON_X, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_Y);
	controller->onButtonEvent(Controller::Key::BUTTON_Y, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_DPAD_UP);
	controller->onButtonEvent(Controller::Key::BUTTON_DPAD_UP, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	controller->onButtonEvent(Controller::Key::BUTTON_DPAD_DOWN, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	controller->onButtonEvent(Controller::Key::BUTTON_DPAD_LEFT, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	controller->onButtonEvent(Controller::Key::BUTTON_DPAD_RIGHT, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
	controller->onButtonEvent(Controller::Key::BUTTON_LEFT_SHOULDER, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	controller->onButtonEvent(Controller::Key::BUTTON_RIGHT_SHOULDER, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_LEFTSTICK);
	controller->onButtonEvent(Controller::Key::BUTTON_LEFT_THUMBSTICK, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
	controller->onButtonEvent(Controller::Key::BUTTON_RIGHT_THUMBSTICK, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_START);
	controller->onButtonEvent(Controller::Key::BUTTON_START, buttonValue ? true : false, buttonValue, false);
	buttonValue = SDL_GameControllerGetButton(sdlController, SDL_CONTROLLER_BUTTON_BACK);
	controller->onButtonEvent(Controller::Key::BUTTON_SELECT, buttonValue ? true : false, buttonValue, false);

	float axis = ((float)SDL_GameControllerGetAxis(sdlController, SDL_CONTROLLER_AXIS_LEFTX)) / AXIS_MAX;
	controller->onAxisEvent(Controller::Key::JOYSTICK_LEFT_X, axis, true);
	axis = ((float)SDL_GameControllerGetAxis(sdlController, SDL_CONTROLLER_AXIS_LEFTY)) / AXIS_MAX;
	controller->onAxisEvent(Controller::Key::JOYSTICK_LEFT_Y, axis, true);
	axis = ((float)SDL_GameControllerGetAxis(sdlController, SDL_CONTROLLER_AXIS_RIGHTX)) / AXIS_MAX;
	controller->onAxisEvent(Controller::Key::JOYSTICK_RIGHT_X, axis, true);
	axis = ((float)SDL_GameControllerGetAxis(sdlController, SDL_CONTROLLER_AXIS_RIGHTY)) / AXIS_MAX;
	controller->onAxisEvent(Controller::Key::JOYSTICK_RIGHT_Y, axis, true);

	// for now we're just treating these as buttons
	axis = ((float)SDL_GameControllerGetAxis(sdlController, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) / AXIS_MAX;
	controller->onButtonEvent(Controller::Key::AXIS_LEFT_TRIGGER, axis > 0.5f ? true : false, axis, false);
	axis = ((float)SDL_GameControllerGetAxis(sdlController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) / AXIS_MAX;
	controller->onButtonEvent(Controller::Key::AXIS_RIGHT_TRIGGER, axis > 0.5f ? true : false, axis, false);

}

Controller::Controller()
	: _controllerTag(TAG_UNSET)
	, _impl(new ControllerImpl())
	, _connectEvent(nullptr)
	, _keyEvent(nullptr)
	, _axisEvent(nullptr)
{
	init();
}

Controller::~Controller()
{
	// TODO - controller
}

void Controller::startDiscoveryController()
{

}

NS_CC_END

#endif // #if (CC_TARGET_PLATFORM == ...)
