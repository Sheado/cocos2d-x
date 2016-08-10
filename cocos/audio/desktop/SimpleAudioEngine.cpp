/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "audio/include/SimpleAudioEngine.h"
#include <unordered_map>
#include <string>
#include "platform/CCFileUtils.h"
#include "SDL_mixer.h"

using namespace cocos2d;
using namespace std;

static bool isSimpleAudioInitialized = false;
static Mix_Music* currentMusic = NULL;  // only one song allowed at a time, therefore we can safely keep this reference here

// TODO - cleanup -
//#include <time.h>
//using namespace std::chrono;
//static long getTimeInMilliseconds() {
//    auto now = system_clock::now();                         //grab time_point now
//    auto now_ms = time_point_cast<milliseconds>(now);       //convert it to milliseconds
//    auto value = now_ms.time_since_epoch();
//    long duration = value.count();
//    return duration;
//}


static void static_init()
{
    if( !isSimpleAudioInitialized )
    {
        Mix_Init(MIX_INIT_OGG);
        Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024);
        isSimpleAudioInitialized = true;
    }
}

static void static_end()
{
}

static void static_preloadBackgroundMusic(const char* pszFilePath)
{
    if( pszFilePath == NULL || strlen( pszFilePath ) <= 0 )
        return;

    currentMusic = Mix_LoadMUS( pszFilePath );
	//if (!currentMusic)
	//	log("Mix_LoadMUS(\"%s\"): %s\n", pszFilePath, Mix_GetError());
}

static void static_playBackgroundMusic(const char* pszFilePath, bool bLoop)
{
    if( pszFilePath == NULL || strlen( pszFilePath ) <= 0 )
        return;
    
    if( currentMusic )
        Mix_PlayMusic(currentMusic, -1);
}

static void static_stopBackgroundMusic()
{
    log( "### TODO: SimpleAudioEngine: static_stopBackgroundMusic()" );
}

static void static_pauseBackgroundMusic()
{
    log( "### TODO: SimpleAudioEngine: static_pauseBackgroundMusic" );
}

static void static_resumeBackgroundMusic()
{
    log( "### TODO: SimpleAudioEngine: static_resumeBackgroundMusic()" );
}

static void static_rewindBackgroundMusic()
{
    log( "### TODO: SimpleAudioEngine: static_rewindBackgroundMusic()" );
}

static bool static_willPlayBackgroundMusic()
{
    log( "### TODO: SimpleAudioEngine: static_willPlayBackgroundMusic()" );
	return false;	// TODO - audio
}

static bool static_isBackgroundMusicPlaying()
{
    log( "### TODO: SimpleAudioEngine: static_isBackgroundMusicPlaying()" );
    return false;
}

static float static_getBackgroundMusicVolume()
{
    log( "### TODO: SimpleAudioEngine: static_getBackgroundMusicVolume()" );
	return 1; // TODO - audio
}

static void static_setBackgroundMusicVolume(float volume)
{
    volume = MAX( MIN(volume, 1.0), 0 );
    volume *= MIX_MAX_VOLUME;
    Mix_VolumeMusic( volume );
}
     
static float static_getEffectsVolume()
{
    log( "### TODO: SimpleAudioEngine: static_getEffectsVolume()" );
	return 1; // TODO - audio
}
     
static void static_setEffectsVolume(float volume)
{
    volume = MAX( MIN(volume, 1.0), 0 );
    volume *= MIX_MAX_VOLUME;
    Mix_Volume(-1,volume);
}

static void static_unloadEffect(const char* pszFilePath)
{
    log( "### TODO: SimpleAudioEngine: static_unloadEffect" );
}

static void static_pauseEffect(unsigned int uSoundId)
{
    log( "### TODO: SimpleAudioEngine: static_pauseEffect" );
}

static void static_pauseAllEffects()
{
    log( "### TODO: SimpleAudioEngine: static_pauseAllEffects" );
}

static void static_resumeEffect(unsigned int uSoundId)
{
    log( "### TODO: SimpleAudioEngine: static_resumeEffect" );
}

static void static_resumeAllEffects()
{
    log( "### TODO: SimpleAudioEngine: static_resumeAllEffects" );
}

namespace CocosDenshion {

static SimpleAudioEngine *s_pEngine;
static unordered_map<string,Mix_Chunk*> audioChunkMap;
    
    
SimpleAudioEngine::SimpleAudioEngine()
{

}

SimpleAudioEngine::~SimpleAudioEngine()
{

}

SimpleAudioEngine* SimpleAudioEngine::getInstance()
{
    static_init();
    if (! s_pEngine)
    {
        s_pEngine = new (std::nothrow) SimpleAudioEngine();
    }
    
    return s_pEngine;
}

void SimpleAudioEngine::end()
{
    if (s_pEngine)
    {
        delete s_pEngine;
        s_pEngine = NULL;
    }
    
    static_end();
}

void SimpleAudioEngine::preloadBackgroundMusic(const char* pszFilePath)
{
    // Changing file path to full path
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(pszFilePath);
    static_preloadBackgroundMusic(fullPath.c_str());
}

void SimpleAudioEngine::playBackgroundMusic(const char* pszFilePath, bool bLoop)
{
    // Changing file path to full path
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(pszFilePath);
    static_playBackgroundMusic(fullPath.c_str(), bLoop);
}

void SimpleAudioEngine::stopBackgroundMusic(bool bReleaseData)
{
    static_stopBackgroundMusic();
}

void SimpleAudioEngine::pauseBackgroundMusic()
{
    static_pauseBackgroundMusic();
}

void SimpleAudioEngine::resumeBackgroundMusic()
{
    static_resumeBackgroundMusic();
} 

void SimpleAudioEngine::rewindBackgroundMusic()
{
    static_rewindBackgroundMusic();
}

bool SimpleAudioEngine::willPlayBackgroundMusic()
{
    return static_willPlayBackgroundMusic();
}

bool SimpleAudioEngine::isBackgroundMusicPlaying()
{
    return static_isBackgroundMusicPlaying();
}

float SimpleAudioEngine::getBackgroundMusicVolume()
{
    return static_getBackgroundMusicVolume();
}

void SimpleAudioEngine::setBackgroundMusicVolume(float volume)
{
    static_setBackgroundMusicVolume(volume);
}

float SimpleAudioEngine::getEffectsVolume()
{
    return static_getEffectsVolume();
}

void SimpleAudioEngine::setEffectsVolume(float volume)
{
    static_setEffectsVolume(volume);
}

unsigned int SimpleAudioEngine::playEffect(const char *pszFilePath, bool bLoop,
                                           float pitch, float pan, float gain)
{
	const int INVALID_CHANNEL = 9999;
	int channel = INVALID_CHANNEL;
    // Changing file path to full path
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(pszFilePath);
    auto it = audioChunkMap.find(pszFilePath);
    if( it != audioChunkMap.end() )
    {
        Mix_Chunk* chunk = it->second;
        if( chunk )
        {
            int loop = 0;
            if( bLoop )
                loop = -1;
			channel = Mix_PlayChannel(-1, chunk, loop);
			if (channel < 0)
				channel = INVALID_CHANNEL;
        }
    }
	return channel;
}

void SimpleAudioEngine::stopEffect(unsigned int nSoundId)
{
	Mix_HaltChannel(nSoundId);
}

void SimpleAudioEngine::preloadEffect(const char* pszFilePath)
{
	auto it = audioChunkMap.find(pszFilePath);
	if (it == audioChunkMap.end())
	{
		// Changing file path to full path
		std::string fullPath = FileUtils::getInstance()->fullPathForFilename(pszFilePath);
		Mix_Chunk* chunk = Mix_LoadWAV(fullPath.data());
		if ( chunk )
			audioChunkMap.insert({ pszFilePath, chunk });
	}
}

void SimpleAudioEngine::unloadEffect(const char* pszFilePath)
{
    // Changing file path to full path
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(pszFilePath);
    static_unloadEffect(fullPath.c_str());
}

void SimpleAudioEngine::pauseEffect(unsigned int uSoundId)
{
    static_pauseEffect(uSoundId);
}

void SimpleAudioEngine::resumeEffect(unsigned int uSoundId)
{
    static_resumeEffect(uSoundId);
}

void SimpleAudioEngine::pauseAllEffects()
{
    static_pauseAllEffects();
}

void SimpleAudioEngine::resumeAllEffects()
{
    static_resumeAllEffects();
}

void SimpleAudioEngine::stopAllEffects()
{
	Mix_HaltChannel(-1);
}
}
