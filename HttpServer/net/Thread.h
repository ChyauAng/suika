#ifndef _THREAD_H
#define _THREAD_H

#include <functional>
#include <string>
#include "notCopyable.h"
#include "CountDownLatch.h"
#include <sys/types.h>

class Thread: public notCopyable{
public:
    typedef std::function<void ()> ThreadFunc;

    // explicit Thread(const ThreadFunc& func, const std::string& name = std::string("Anonymous"));
    explicit Thread(ThreadFunc&& func, const std::string& name = std::string("Anonymous"));
    ~Thread();

    void start();
    int join();

    bool started() const{return started_;};
    pid_t tid();
    const std::string& name() const{
        return name_;
    }

private:
    bool started_;
    bool joined_;
    pid_t tid_;
    pthread_t pthreadId_;
    ThreadFunc func_;
    CountDownLatch latch_;
    std::string name_;

};


#endif
