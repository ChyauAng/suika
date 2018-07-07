#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "CurrentThread.h"
#include "notCopyable.h"
#include <assert.h>
#include <pthread.h>


class MutexLock:public notCopyable{
public:
    MutexLock()
        :holder_(0){
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock(){
        assert(0 == holder_);
        pthread_mutex_destroy(&mutex_);
    }

    void lock(){
        pthread_mutex_lock(&mutex_);
        assignHolder();
    }

    void unlock(){
        unassignHolder();
        pthread_mutex_unlock(&mutex_);
    }

    bool isLockedByThisThread() const{
        return holder_ == CurrentThread::tid();
    }

    void assertLocked() const{
        assert(isLockedByThisThread());
    }
    
    // why is non-const?
    pthread_mutex_t* getMutex(){
        return &mutex_;
    }
    
    /*
    pthread_mutex_t* getMutex() const{
        return &mutex_;
    }
    */

private:
    friend class Condition;
    
    // conditional variable
    class UnassignGuard: public notCopyable{
    public:
        UnassignGuard(MutexLock& owner)
            :owner_(owner){
                owner_.unassignHolder();
        }

        ~UnassignGuard(){
            owner_.assignHolder();
        }

    private:
        MutexLock& owner_;
    };

    void unassignHolder(){
        holder_ = 0;
    }

    void assignHolder(){
        holder_ = CurrentThread::tid();
    }

    pthread_mutex_t mutex_;
    pid_t holder_;
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
