#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <assert.h>
#include <pthread.h>

#include "CurrentThread.h"
#include "notCopyable.h"


class MutexLock: public notCopyable{
public:
    MutexLock(){
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock(){
        pthread_mutex_destroy(&mutex_);
    }

    void lock(){
        pthread_mutex_lock(&mutex_);
    }

    void unlock(){
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* getMutex(){
        return &mutex_;
    }

private:
    friend class Condition;
    
    pthread_mutex_t mutex_;
};

// stack object
// RAII
class MutexLockGuard: public notCopyable{
public:
    explicit MutexLockGuard(MutexLock& mutex)
        :mutex_(mutex){
        mutex_.lock();
    }

    ~MutexLockGuard(){
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;
};

#endif
