#ifndef HEADER_GUARD_AUDIO_SINK_H
#define HEADER_GUARD_AUDIO_SINK_H

namespace JAZZROS {

    class AudioSink {
    public:
        virtual void play() = 0;

        virtual void pause() = 0;

        virtual void stop() = 0;

        virtual bool playing() const = 0;

        virtual double getDelay() const = 0;

        virtual void setDelay(const double delay) = 0;

        virtual void setVolume(float) = 0;

        virtual float getVolume() const = 0;
    };
} // namespace JAZZROS

#endif // HEADER_GUARD_AUDIO_SINK_H