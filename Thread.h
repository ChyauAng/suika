#ifndef _THREAD_H
#define _THREAD_H

#include<functional>
#include<pthread.h>
#include"notCopyable.h"

class Thread: public notCopyable{
public:
    typedef std::function<void ()> ThreadFunc;

    Thread(const ThreadFunc&);
    ~Thread();

    void start();
    int join();

    bool started();
    pid_t tid();

private:
    bool started_;
    bool joined_;
    pid_t tid_;
    pthread_t pthreadId_;
    ThreadFunc func_;

};


#endif
