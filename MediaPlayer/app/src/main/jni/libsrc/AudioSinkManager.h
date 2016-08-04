#ifndef __AUDIOSINKMANAGER_H__
#define __AUDIOSINKMANAGER_H__


namespace JAZZROS {
    class AudioSink;
    class AudioStream;
}; // namespace JAZZROS

class AudioSinkManager
{
private:
    static JAZZROS::AudioStream * m_lastSetSDLAudioStream;


    static JAZZROS::AudioSink * BuildSDLMuteAudioSink(JAZZROS::AudioStream* audioStream);

    static JAZZROS::AudioSink * BuildSDLAudioSink(JAZZROS::AudioStream* audioStream);

    AudioSinkManager();
public:
    ~AudioSinkManager();


    static void  setSDLAudioSink(JAZZROS::AudioStream* audioStream);
};


#endif // __AUDIOSINKMANAGER_H__
