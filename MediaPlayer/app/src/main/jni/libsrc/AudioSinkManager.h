#ifndef __AUDIOSINKMANAGER_H__
#define __AUDIOSINKMANAGER_H__


namespace JAZZROS {
    class AudioSink;
    class AudioStream;
    class FFmpegPlayer;
}; // namespace JAZZROS

class AudioSinkManager
{
private:
    static JAZZROS::FFmpegPlayer * m_lastSetSDLAudioPlayer;


    static JAZZROS::AudioSink * BuildSDLMuteAudioSink(JAZZROS::AudioStream* audioStream);

    static JAZZROS::AudioSink * BuildSDLProxiAudioSink(JAZZROS::AudioStream* audioStream);

    AudioSinkManager();
public:
    ~AudioSinkManager();

    static const int initialize();
    static const int release();

    static void  setSDLAudioSink(JAZZROS::FFmpegPlayer* player);
};


#endif // __AUDIOSINKMANAGER_H__
