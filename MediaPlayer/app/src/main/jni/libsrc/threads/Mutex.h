#ifndef HEADER_GUARD_MUTEX_H
#define HEADER_GUARD_MUTEX_H

#include <pthread.h>

namespace JAZZROS {
    class Mutex {
        pthread_mutex_t *mutexlock;

    public:
        Mutex()
                : mutexlock(new pthread_mutex_t()) {

            pthread_mutex_init(mutexlock, 0);
        }

        ~Mutex() {
            pthread_mutex_unlock(mutexlock);
            delete mutexlock;
        }

        pthread_mutex_t *get() {
            return mutexlock;
        }
    };
} // JAZZROS
#endif // HEADER_GUARD_MUTEX_H
