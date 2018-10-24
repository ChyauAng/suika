#ifndef _THREAD_H
#define _THREAD_H

#include "CountDownLatch.h"
#include "notCopyable.h"

#include <functional>
#include <sys/types.h>

class Thread: public notCopyable{
public:
    typedef std::function<void ()> ThreadFunc;

    explicit Thread(ThreadFunc&& func);
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
