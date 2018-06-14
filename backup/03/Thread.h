#ifndef _THREAD_H
#define _THREAD_H

#include <functional>
#include <pthread.h>
#include <string.h>
#include "notCopyable.h"
#include "CountDownLatch.h"

class Thread: public notCopyable{
public:
    typedef std::function<void ()> ThreadFunc;

    explicit Thread(const ThreadFunc&);
    ~Thread();

    void start();
    int join();

    bool started() const{return started_;};
    pid_t tid();

private:
    bool started_;
    bool joined_;
    pid_t tid_;
    pthread_t pthreadId_;
    ThreadFunc func_;
    CountDownLatch latch_;

};


#endif
