#ifndef HEADER_GUARD_SDL_MUTEAUDIOSINK_H
#define HEADER_GUARD_SDL_MUTEAUDIOSINK_H

#include "devices/AudioSink.hpp"
#include "AudioSinkManager.h"
#include <SDL.h>

namespace JAZZROS {
    class AudioStream;
}; // using namespace JAZZROS

class SDLMuteAudioSink : public JAZZROS::AudioSink
{
    friend class AudioSinkManager;

    SDL_TimerID                         m_timer_id;
    JAZZROS::AudioStream                * _audioStream;

    virtual double getDelay() const {return 0.0;}
    virtual void setDelay(const double delay) {}
    virtual void setVolume(float) {}
    virtual float getVolume() const {return 0.5f;};

    // Only class AudioSinkManager may construct this object
    SDLMuteAudioSink(JAZZROS::AudioStream* audioStream):
            m_timer_id(0),
            _audioStream(audioStream) {
    }
public:

    ~SDLMuteAudioSink();

    virtual void play();
    virtual void pause();
    virtual void stop();

    virtual bool playing() const { return m_timer_id != 0; }
};

#endif // HEADER_GUARD_SDL_MUTEAUDIOSINK_H
