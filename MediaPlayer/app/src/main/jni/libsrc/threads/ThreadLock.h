#ifndef HEADER_GUARD_THREADLOCK_H
#define HEADER_GUARD_THREADLOCK_H

#include <pthread.h>

// todo: could be replaced by crossplatform OpenThreads::Mutex
struct CThreadLock
{
    pthread_mutex_t mutexlock;


    CThreadLock();
    virtual ~CThreadLock();

    void Lock();
    void Unlock();
};

CThreadLock::CThreadLock()
{
    // init lock here
    pthread_mutex_init(&mutexlock, 0);
}

CThreadLock::~CThreadLock()
{
    // deinit lock here
    pthread_mutex_destroy(&mutexlock);
}

void CThreadLock::Lock()
{
    // lock
    pthread_mutex_lock(&mutexlock);
}

void CThreadLock::Unlock()
{
    // unlock
    pthread_mutex_unlock(&mutexlock);
}

#endif // HEADER_GUARD_THREADLOCK_H