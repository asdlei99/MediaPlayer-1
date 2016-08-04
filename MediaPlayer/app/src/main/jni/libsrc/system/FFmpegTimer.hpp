#ifndef HEADER_GUARD_FFMPEG_TIMER_H
#define HEADER_GUARD_FFMPEG_TIMER_H

#include "Timer.h"

namespace JAZZROS {

class FFmpegTimer
{
    Timer                   m_timer;
    unsigned long           m_start_time_ms;
    unsigned long           m_stop_time_ms;
    unsigned long           m_offset;
public:
                        FFmpegTimer();

    void                Start();
    void                Stop();
    void                Reset();

    void                ElapsedMilliseconds(const unsigned long & ms);
    const unsigned long ElapsedMilliseconds() const;
};

} // namespace JAZZROS

#endif // HEADER_GUARD_FFMPEG_TIMER_H