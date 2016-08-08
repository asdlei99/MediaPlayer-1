#include "AudioSinkManager.h"

#include "devices/AudioStream.hpp"

#include "SDLMuteAudioSink.h"
#include "SDLProxiAudioSink.h"
#include "SDLAudioSink.h"
#include "FFmpegPlayer.hpp"


JAZZROS::FFmpegPlayer * AudioSinkManager::m_lastSetSDLAudioPlayer = NULL;
SDLAudioSink        m_sdl_audiosink(NULL);

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
AudioSinkManager::BuildSDLProxiAudioSink(JAZZROS::AudioStream* audioStream)
{
    return new SDLProxiAudioSink(audioStream, & m_sdl_audiosink);
}

void
AudioSinkManager::setSDLAudioSink(JAZZROS::FFmpegPlayer* player)
{
    // Prevent reassigning the same sink
    if (m_lastSetSDLAudioPlayer == player)
        return;

    JAZZROS::AudioStream *   lastAudioStream = NULL;
    JAZZROS::AudioStream *   newAudioStream = NULL;

    if (player->getAudioStreams().empty() == false)
        newAudioStream = player->getAudioStreams().front();

    // Turn off the last SDL audio sink
    if (m_lastSetSDLAudioPlayer != NULL)
    {
        if (m_lastSetSDLAudioPlayer->getAudioStreams().empty() == false)
            lastAudioStream = m_lastSetSDLAudioPlayer->getAudioStreams().front();

        if (lastAudioStream)
            JAZZROS::FFmpegPlayer::setAudioSink (m_lastSetSDLAudioPlayer,
                                                 lastAudioStream,
                                                 AudioSinkManager::BuildSDLMuteAudioSink (lastAudioStream) );
    }


    m_lastSetSDLAudioPlayer = player;

    if (newAudioStream)
        newAudioStream->setAudioSink ( AudioSinkManager::BuildSDLProxiAudioSink(newAudioStream) );

}
