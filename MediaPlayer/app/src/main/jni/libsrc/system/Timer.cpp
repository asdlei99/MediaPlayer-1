#include <stdio.h>
#include <string.h>

#include "Timer.h"

using namespace JAZZROS;


Timer* Timer::instance()
{
    static Timer s_timer;
    return &s_timer;
}

#ifdef WIN32

    #include <sys/types.h>
    #include <fcntl.h>
    #include <windows.h>
    #include <winbase.h>
    Timer::Timer()
    {
        LARGE_INTEGER frequency;
        if(QueryPerformanceFrequency(&frequency))
        {
            _secsPerTick = 1.0/(double)frequency.QuadPart;
        }
        else
        {
            _secsPerTick = 1.0;
            notify(NOTICE)<<"Error: Timer::Timer() unable to use QueryPerformanceFrequency, "<<std::endl;
            notify(NOTICE)<<"timing code will be wrong, Windows error code: "<<GetLastError()<<std::endl;
        }
       
        setStartTick();       
    }

    Timer_t Timer::tick() const
    {
        LARGE_INTEGER qpc;
        if (QueryPerformanceCounter(&qpc))
        {
            return qpc.QuadPart;
        }
        else
        {
            notify(NOTICE)<<"Error: Timer::Timer() unable to use QueryPerformanceCounter, "<<std::endl;
            notify(NOTICE)<<"timing code will be wrong, Windows error code: "<<GetLastError()<<std::endl;
            return 0;
        }
    }

#else
    #include <unistd.h>

    Timer::Timer( void )
    {
        _secsPerTick = (1.0 / (double) 1000000);

        setStartTick();       
    }

    #if defined(_POSIX_TIMERS) && ( _POSIX_TIMERS > 0 ) && defined(_POSIX_MONOTONIC_CLOCK)
        #include <time.h>

        Timer_t Timer::tick() const
        {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return ((JAZZROS::Timer_t)ts.tv_sec)*1000000+(JAZZROS::Timer_t)ts.tv_nsec/1000;
        }
    #else
        #include <sys/time.h>

        Timer_t Timer::tick() const
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return ((JAZZROS::Timer_t)tv.tv_sec)*1000000+(JAZZROS::Timer_t)tv.tv_usec;
        }
    #endif

#endif