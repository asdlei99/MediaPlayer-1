#ifndef HEADER_GUARD_SDL_AUDIOSINK_H
#define HEADER_GUARD_SDL_AUDIOSINK_H


#include "devices/AudioSink.hpp"
#include "AudioSinkManager.h"
#include <SDL.h>

namespace JAZZROS {
    class AudioStream;
}; // using namespace JAZZROS

class SDLAudioSink : public JAZZROS::AudioSink
{
    friend class AudioSinkManager;

    virtual double getDelay() const {return 0.0;}
    virtual void setDelay(const double delay) {}
    virtual void setVolume(float) {}
    virtual float getVolume() const {return 0.5f;};

public:
    SDLAudioSink(JAZZROS::AudioStream* audioStream):
            _started(false),
            _paused(false),
            _audioStream(audioStream) {
    }

    ~SDLAudioSink();

    virtual void play();
    virtual void pause();
    virtual void stop();

    virtual bool playing() const { return _started && !_paused; }


    bool                                _started;
    bool                                _paused;
    JAZZROS::AudioStream                * _audioStream;
};

#endif // HEADER_GUARD_SDL_AUDIOSINK_H
