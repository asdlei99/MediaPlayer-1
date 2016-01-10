#ifndef HEADER_GUARD_FFMPEG_AUDIO_STREAM_H
#define HEADER_GUARD_FFMPEG_AUDIO_STREAM_H

#include "../devices/AudioStream.hpp"

namespace JAZZROS
{
    class FFmpegFileHolder;
    class FFmpegStreamer;

    class FFmpegAudioStream : public AudioStream
    {
    public:

                                        FFmpegAudioStream (FFmpegFileHolder * pFileHolder = NULL,
                                                        FFmpegStreamer * pStreamer = NULL);

                                        FFmpegAudioStream(const FFmpegAudioStream & audio); // todo: actually it will be good to hide copy-operators

        virtual void                    setAudioSink(AudioSink* audio_sink);

        virtual void                    consumeAudioBuffer(void * const buffer,
                                                        const size_t size);

        virtual int                     audioFrequency() const;
        virtual int                     audioNbChannels() const;
        virtual AudioStream::SampleFormat audioSampleFormat() const;

        double                          duration() const;

    private:

        virtual                         ~FFmpegAudioStream();

        FFmpegFileHolder *              m_pFileHolder;
        FFmpegStreamer *                m_streamer;

    };

} // namespace JAZZROS

#endif // HEADER_GUARD_FFMPEG_AUDIO_STREAM_H
