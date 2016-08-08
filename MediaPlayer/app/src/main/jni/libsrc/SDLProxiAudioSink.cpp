#include "SDLProxiAudioSink.h"
#include "SDLAudioSink.h"

SDLProxiAudioSink::SDLProxiAudioSink(JAZZROS::AudioStream* audioStream, JAZZROS::AudioSink * sink)
{
    SDLAudioSink * sdl_sink = dynamic_cast<SDLAudioSink*>(sink);

    if (sdl_sink)
    {
        bool isPlayied = false;

        if (sdl_sink->playing())
        {
            isPlayied = true;
            sdl_sink->pause();
        }
        //
        sdl_sink->_audioStream = audioStream;
        m_sink = sdl_sink;

        if (isPlayied)
            sdl_sink->play();
    }
}

SDLProxiAudioSink::~SDLProxiAudioSink()
{
}

double SDLProxiAudioSink::getDelay() const
{
    if (m_sink)
    {
        return m_sink->getDelay();
    }
}
void SDLProxiAudioSink::setDelay(const double delay)
{
    if (m_sink)
    {
        m_sink->setDelay(delay);
    }
}
void SDLProxiAudioSink::setVolume(float value)
{
    if (m_sink)
    {
        m_sink->setVolume(value);
    }
}
float SDLProxiAudioSink::getVolume() const
{
    if (m_sink)
    {
        return m_sink->getVolume();
    }
}
void SDLProxiAudioSink::play()
{
    if (m_sink)
    {
        m_sink->play();
    }
}
void SDLProxiAudioSink::pause()
{
    if (m_sink)
    {
        m_sink->pause();
    }
}
void SDLProxiAudioSink::stop()
{
    if (m_sink)
    {
        m_sink->stop();
    }
}
bool SDLProxiAudioSink::playing() const
{
    if (m_sink)
    {
        return m_sink->playing();
    }
}
