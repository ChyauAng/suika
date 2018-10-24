#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <errno.h>
#include <pthread.h>
#include <stdint.h>

#include "notCopyable.h"
#include "Mutex.h"


class Condition: public notCopyable{
public:
    explicit Condition(MutexLock& mutex)
        :mutex_(mutex){
        pthread_cond_init(&pcond_, NULL);
    }

    ~Condition(){
        pthread_cond_destroy(&pcond_);
    }

    void wait(){
        pthread_cond_wait(&pcond_, mutex_.getMutex());
    }

    bool waitForSeconds(int seconds){
        struct timespec relativetime;
        clock_gettime(CLOCK_MONOTONIC, &relativetime);


        relativetime.tv_sec += static_cast<time_t>(seconds);

        return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getMutex(), &relativetime);
    }

    void notify(){
        pthread_cond_signal(&pcond_);
    }

    void notifyAll(){
        pthread_cond_broadcast(&pcond_);
    }

private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};

#endif
