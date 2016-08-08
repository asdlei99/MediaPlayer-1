#ifndef SDLPROXIAUDIOSINK_H
#define SDLPROXIAUDIOSINK_H

#include "devices/AudioSink.hpp"
#include "AudioSinkManager.h"
#include <SDL.h>

namespace JAZZROS {
    class AudioStream;
}; // using namespace JAZZROS

//class SDLAudioSink;
class SDLProxiAudioSink : public JAZZROS::AudioSink
{
private:
    friend class AudioSinkManager;

    JAZZROS::AudioSink * m_sink;

    virtual double getDelay() const;
    virtual void setDelay(const double delay);
    virtual void setVolume(float);
    virtual float getVolume() const;
    virtual void play();
    virtual void pause();
    virtual void stop();
    virtual bool playing() const;

    // Only class AudioSinkManager may construct this object
    SDLProxiAudioSink(JAZZROS::AudioStream* audioStream, JAZZROS::AudioSink * sink);

public:
    virtual ~SDLProxiAudioSink();
};

#endif // SDLPROXIAUDIOSINK_H
