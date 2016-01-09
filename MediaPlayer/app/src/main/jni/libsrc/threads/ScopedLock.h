#ifndef HEADER_GUARD_SCOPEDLOCK_H
#define HEADER_GUARD_SCOPEDLOCK_H

#include <OpenThreads/Mutex>

namespace JAZZROS {

    template<class M>
    class TScopedLock {
    private:
        M &m_lock;

        TScopedLock(const TScopedLock &); // prevent copy
        TScopedLock &operator=(const TScopedLock &); // prevent assign
    public:
        explicit TScopedLock(M &m) : m_lock(m) { m_lock.lock(); }

        ~TScopedLock() { m_lock.unlock(); }
    };

    template<class M>
    class ReverseTScopedLock {
    private:
        M &m_lock;

        ReverseTScopedLock(const ReverseTScopedLock &); // prevent copy
        ReverseTScopedLock &operator=(const ReverseTScopedLock &); // prevent assign
    public:
        explicit ReverseTScopedLock(M &m) : m_lock(m) { m_lock.unlock(); }

        ~ReverseTScopedLock() { m_lock.lock(); }
    };
} // JAZZROS

#endif // HEADER_GUARD_SCOPEDLOCK_H