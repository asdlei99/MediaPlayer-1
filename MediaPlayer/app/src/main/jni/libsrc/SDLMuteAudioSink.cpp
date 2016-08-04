#include "SDLMuteAudioSink.h"
#include "devices/AudioStream.hpp"
#include <string>
#include <stdio.h>

#define  LOG_TAG    "SDL"

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>

#define  LOG(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#define  LOGI(...)  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#define  LOGE(...)  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#define  ASSETS_DIR ""

#else

#define  LOG(...)   printf(__VA_ARGS__);printf("\n");
#define  LOGI(...)  printf(__VA_ARGS__);printf("\n");
#define  LOGE(...)  printf(__VA_ARGS__);printf("\n");
#define  ASSETS_DIR "assets/"

#endif

#include "threads/ScopedLock.h"
typedef OpenThreads::Mutex          Mutex;
typedef JAZZROS::TScopedLock<Mutex> ScopedLock;
Mutex                               g_lockSDLMuteAudioSinkMutex;


#define MUTE_BUF_SIZE   (2 * 44100 * 2 / 10)
unsigned char MUTE_BUFFER[MUTE_BUF_SIZE * 2]; // multimply by 2 for memory enough guaranties

static Uint32 soundReadCallback(Uint32 interval, void *param)
{
    ScopedLock  lock(g_lockSDLMuteAudioSinkMutex);

    JAZZROS::AudioStream * ptr = reinterpret_cast<JAZZROS::AudioStream*>(param);

    if (ptr)
        ptr->consumeAudioBuffer(MUTE_BUFFER, MUTE_BUF_SIZE);

    return(interval);
}

SDLMuteAudioSink::~SDLMuteAudioSink()
{
    stop();
}

void SDLMuteAudioSink::play()
{
    ScopedLock  lock(g_lockSDLMuteAudioSinkMutex);

    if (m_timer_id != 0)
        return;
/*
    LOG("SDLMuteAudioSink()::startPlaying()");

    LOG("  audioFrequency()=%d", _audioStream->audioFrequency());
    LOG("  audioNbChannels()=%d", _audioStream->audioNbChannels());
    LOG("  bytePerSample()=%d", _audioStream->bytePerSample());
    LOG("  audioSampleFormat()=%d", _audioStream->audioSampleFormat());
*/
    //
    // Initiate timer to send audio samples
    // with real-time speed.
    //
    Uint32 bytesPerSec = _audioStream->audioNbChannels() * _audioStream->audioFrequency() * _audioStream->bytePerSample();
    Uint32 timeRange = 1000 * MUTE_BUF_SIZE / bytesPerSec;

    m_timer_id = SDL_AddTimer (timeRange, soundReadCallback, _audioStream);

//    LOG("SDLMuteAudioSink::play() finished successfully");
}

void SDLMuteAudioSink::pause()
{
    stop();
}

void SDLMuteAudioSink::stop()
{
    ScopedLock  lock(g_lockSDLMuteAudioSinkMutex);

    if (m_timer_id != 0)
    {
        const SDL_bool success = SDL_RemoveTimer (m_timer_id);
    }

    m_timer_id = 0;
}
