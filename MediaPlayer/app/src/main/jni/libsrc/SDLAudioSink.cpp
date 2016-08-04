#include "SDLAudioSink.h"
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


static void soundReadCallback(void * user_data, uint8_t * data, int datalen)
{
    SDLAudioSink * sink = reinterpret_cast<SDLAudioSink*>(user_data);

    if (sink->_audioStream)
        sink->_audioStream->consumeAudioBuffer(data, datalen);
}

SDLAudioSink::~SDLAudioSink()
{
    stop();
}

void SDLAudioSink::play()
{
    if (_started)
    {
        if (_paused)
        {
            SDL_PauseAudio(0);
            _paused = false;
        }
        return;
    }

    _started = true;
    _paused = false;
/*
    LOG("SDLAudioSink()::startPlaying()");

    LOG("  audioFrequency()=%d", _audioStream->audioFrequency());
    LOG("  audioNbChannels()=%d", _audioStream->audioNbChannels());
    LOG("  bytePerSample()=%d", _audioStream->bytePerSample());
    LOG("  audioSampleFormat()=%d", _audioStream->audioSampleFormat());
*/



    SDL_AudioSpec specs = { 0 };
    SDL_AudioSpec wanted_specs = { 0 };

    wanted_specs.freq = _audioStream->audioFrequency();
    wanted_specs.channels = _audioStream->audioNbChannels();
    wanted_specs.silence = 0;
    wanted_specs.samples = 1024;
    wanted_specs.callback = soundReadCallback;
    wanted_specs.userdata = this;
    wanted_specs.format = 0;

#if SDL_MAJOR_VERSION>=2
    switch(_audioStream->audioSampleFormat())
    {
        case(JAZZROS::AudioStream::SAMPLE_FORMAT_U8):  { wanted_specs.format = AUDIO_U8; LOG("    SampleFormat  = SAMPLE_FORMAT_U8");  break;}
        case(JAZZROS::AudioStream::SAMPLE_FORMAT_S16): { wanted_specs.format = AUDIO_S16SYS; LOG("    SampleFormat  = SAMPLE_FORMAT_S16"); break; }
        case(JAZZROS::AudioStream::SAMPLE_FORMAT_S24): { LOG("    SampleFormat  = SAMPLE_FORMAT_S24. Not support by SDL"); break; }
        case(JAZZROS::AudioStream::SAMPLE_FORMAT_S32): { wanted_specs.format = AUDIO_S32SYS; LOG("    SampleFormat  = SAMPLE_FORMAT_S32"); break; }
        case(JAZZROS::AudioStream::SAMPLE_FORMAT_F32): { wanted_specs.format = AUDIO_F32SYS; LOG("    SampleFormat  = SAMPLE_FORMAT_F32"); break; }
    }
#else
    switch(_audioStream->audioSampleFormat())
    {
        case(JAZZROS::AudioStream::SAMPLE_FORMAT_U8):  { wanted_specs.format = AUDIO_U8; LOG("    SampleFormat  = SAMPLE_FORMAT_U8");  break;}
        case(JAZZROS::AudioStream::SAMPLE_FORMAT_S16): { wanted_specs.format = AUDIO_S16SYS; LOG("    SampleFormat  = SAMPLE_FORMAT_S16"); break; }
        case(JAZZROS::AudioStream::SAMPLE_FORMAT_S24): { LOG("    SampleFormat  = SAMPLE_FORMAT_S24. Not support by SDL1.x"); break; }
        case(JAZZROS::AudioStream::SAMPLE_FORMAT_S32): { LOG("    SampleFormat  = SAMPLE_FORMAT_S32. Not support by SDL1.x"); break; }
        case(JAZZROS::AudioStream::SAMPLE_FORMAT_F32): { LOG("    SampleFormat  = SAMPLE_FORMAT_F32. Not support by SDL1.x"); break; }
    }
#endif

    if (wanted_specs.format!=0)
    {
//        LOG("SDLAudioSink() -> SDL_OpenAudio()...");
        if (SDL_OpenAudio(&wanted_specs, &specs) < 0) // todo: actually real params stored in \specs instead of \wanted_specs. And it should be processed
            throw "SDL_OpenAudio() failed (" + std::string(SDL_GetError()) + ")";

//        LOG("SDLAudioSink() -> SDL_PauseAudio...");
        SDL_PauseAudio(0);
    }
    else
    {
        throw "SDL_OpenAudio() does not support audio format";
    }
//    LOG("SDLAudioSink::play() finished successfully");
}

void SDLAudioSink::pause()
{
    if (_started)
    {
        SDL_PauseAudio(1);
        _paused = true;
    }
}

void SDLAudioSink::stop()
{
    if (_started)
    {
        if (!_paused) SDL_PauseAudio(1);
        SDL_CloseAudio();

//        LOG("~SDLAudioSink() destructor, but still playing");
    }
}
