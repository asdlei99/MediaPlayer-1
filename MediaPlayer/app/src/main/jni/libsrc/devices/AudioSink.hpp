#ifndef HEADER_GUARD_AUDIO_SINK_H
#define HEADER_GUARD_AUDIO_SINK_H

namespace JAZZROS {

    class AudioSink {
    public:
        virtual void play() = 0;

        virtual void pause() = 0;

        virtual void stop() = 0;

        virtual bool playing() const = 0;

        virtual double getDelay() const;

        virtual void setDelay(const double delay);

        virtual void setVolume(float);

        virtual float getVolume() const;
    };
} // namespace JAZZROS

#endif // HEADER_GUARD_AUDIO_SINK_H