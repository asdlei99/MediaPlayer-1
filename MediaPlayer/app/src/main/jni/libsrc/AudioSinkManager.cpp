#include "AudioSinkManager.h"

#include "devices/AudioStream.hpp"

#include "SDLMuteAudioSink.h"
#include "SDLAudioSink.h"



JAZZROS::AudioStream * AudioSinkManager::m_lastSetSDLAudioStream = NULL;

AudioSinkManager::AudioSinkManager()
{
}

AudioSinkManager::~AudioSinkManager()
{
}

JAZZROS::AudioSink *
AudioSinkManager::BuildSDLMuteAudioSink(JAZZROS::AudioStream* audioStream)
{
    return new SDLMuteAudioSink(audioStream);
}

JAZZROS::AudioSink *
AudioSinkManager::BuildSDLAudioSink(JAZZROS::AudioStream* audioStream)
{
    return new SDLAudioSink(audioStream);
}

void
AudioSinkManager::setSDLAudioSink(JAZZROS::AudioStream* audioStream)
{
    // Prevent reassigning the same sink
    if (m_lastSetSDLAudioStream == audioStream)
        return;

    // Turn off the last SDL audio sink
    if (m_lastSetSDLAudioStream != NULL)
        m_lastSetSDLAudioStream->setAudioSink ( AudioSinkManager::BuildSDLMuteAudioSink(m_lastSetSDLAudioStream) );

    m_lastSetSDLAudioStream = audioStream;

    if (audioStream)
        audioStream->setAudioSink ( AudioSinkManager::BuildSDLAudioSink(audioStream) );
}
