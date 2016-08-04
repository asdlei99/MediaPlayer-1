#ifndef HEADER_GUARD_AUDIO_STREAM_H
#define HEADER_GUARD_AUDIO_STREAM_H

#include <stddef.h>

namespace JAZZROS {

    class AudioSink;

    class AudioStream {
    public :
        enum SampleFormat {
            SAMPLE_FORMAT_U8,
            SAMPLE_FORMAT_S16,
            SAMPLE_FORMAT_S24,
            SAMPLE_FORMAT_S32,
            SAMPLE_FORMAT_F32
        };

        virtual void setAudioSink(AudioSink *audio_sink) = 0;

        virtual void consumeAudioBuffer(void *const buffer, const size_t size) = 0;

        virtual int audioFrequency() const = 0;

        virtual int audioNbChannels() const = 0;

        virtual int bytePerSample() const = 0;

        virtual SampleFormat audioSampleFormat() const = 0;
    };

} // namespace JAZZROS

#endif // HEADER_GUARD_AUDIO_STREAM_H

