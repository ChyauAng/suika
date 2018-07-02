#ifndef _EVENTLOOPTHREAD_H_
#define _EVENTLOOPTHREAD_H_

#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"
#include "notCopyable.h"

class EventLoop;

// one event loop per thread
// start an IO thread then waiting for events coming
class EventLoopThread:public notCopyable{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;
    EventLoopThread();
    // EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop* loop_;
    bool existing_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    ThreadInitCallback callback_;
};

#endif
