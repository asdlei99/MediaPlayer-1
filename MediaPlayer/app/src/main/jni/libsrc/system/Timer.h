#ifndef HEADER_GUARD_TIMER_H
#define HEADER_GUARD_TIMER_H

namespace JAZZROS {

#if defined(_MSC_VER)
    typedef __int64 Timer_t;
#else
    typedef unsigned long long Timer_t;
#endif

    class Timer {

    public:

        Timer();

        ~Timer() { }

        static Timer *instance();

        /** Get the timers tick value.*/
        Timer_t tick() const;

        /** Set the start.*/
        void setStartTick() { _startTick = tick(); }

        void setStartTick(Timer_t t) { _startTick = t; }

        Timer_t getStartTick() const { return _startTick; }


        /** Get elapsed time in seconds.*/
        inline double time_s() const { return delta_s(_startTick, tick()); }

        /** Get elapsed time in milliseconds.*/
        inline double time_m() const { return delta_m(_startTick, tick()); }

        /** Get elapsed time in micoseconds.*/
        inline double time_u() const { return delta_u(_startTick, tick()); }

        /** Get elapsed time in nanoseconds.*/
        inline double time_n() const { return delta_n(_startTick, tick()); }

        /** Get the time in seconds between timer ticks t1 and t2.*/
        inline double delta_s(Timer_t t1, Timer_t t2) const {
            return (double) (t2 - t1) * _secsPerTick;
        }

        /** Get the time in milliseconds between timer ticks t1 and t2.*/
        inline double delta_m(Timer_t t1, Timer_t t2) const { return delta_s(t1, t2) * 1e3; }

        /** Get the time in microseconds between timer ticks t1 and t2.*/
        inline double delta_u(Timer_t t1, Timer_t t2) const { return delta_s(t1, t2) * 1e6; }

        /** Get the time in nanoseconds between timer ticks t1 and t2.*/
        inline double delta_n(Timer_t t1, Timer_t t2) const { return delta_s(t1, t2) * 1e9; }

        /** Get the the number of seconds per tick. */
        inline double getSecondsPerTick() const { return _secsPerTick; }

    protected :

        Timer_t _startTick;
        double _secsPerTick;
    };

/** Helper class for timing sections of code. */
    class ElapsedTime {
    public:
        inline ElapsedTime(double *elapsedTime, JAZZROS::Timer *timer = 0) :
                _time(elapsedTime) {
            init(timer);
        }

        inline ElapsedTime(JAZZROS::Timer *timer = 0) :
                _time(0) {
            init(timer);
        }

        inline ~ElapsedTime() {
            finish();
        }

        inline void reset() {
            _startTick = _timer->tick();
        }

        inline double elapsedTime() const {
            return _timer->delta_s(_startTick, _timer->tick());
        }

        inline void finish() {
            Timer_t endTick = _timer->tick();
            if (_time) *_time += _timer->delta_s(_startTick, endTick);
            _startTick = endTick;
        }

    protected:

        inline void init(JAZZROS::Timer *timer) {
            if (timer) _timer = timer;
            else _timer = JAZZROS::Timer::instance();

            _startTick = _timer->tick();
        }

        double *_time;
        Timer *_timer;
        Timer_t _startTick;

    };

} // namespace JAZZROS

#endif // HEADER_GUARD_TIMER_H