#ifndef HEADER_GUARD_SCOPEDLOCK_H
#define HEADER_GUARD_SCOPEDLOCK_H

#include "Mutex.h"

namespace JAZZROS {

    class ScopedLock {
    private:
        Mutex m_mutex;

        ScopedLock(const ScopedLock &copy);

        ScopedLock &operator=(const ScopedLock &rhs);

    public:
        ScopedLock(Mutex &m) : m_mutex(m) {
            pthread_mutex_lock(m_mutex.get());
        }

        ~ScopedLock() {
            pthread_mutex_unlock(m_mutex.get());
        }

        void wait(pthread_cond_t *cond) {
            int rc = 0;

            rc = pthread_cond_wait(cond, m_mutex.get());
        }
    };
} // JAZZROS

#endif // HEADER_GUARD_SCOPEDLOCK_H