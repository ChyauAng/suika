#ifndef _EVENTLOOPTHREADPOOL_H_
#define _EVENTLOOPTHREADPOOL_H_

#include <functional>
#include <vector>

#include "base/notCopyable.h"

class EventLoop;
class EventLoopThread;
class EventLoopThreadPool;

// stack variable
class EventLoopThreadPool: public notCopyable{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThreadPool(EventLoop* loop);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads){
        numThreads_ = numThreads;
    }

    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    EventLoop* getNextLoop();
    std::vector<EventLoop*> getAllLoops();

    bool started() const{
        return started_;
    }


private:
    bool started_;

    int numThreads_;
    int next_;

    EventLoop* baseLoop_;

    std::vector<EventLoopThread*> threads_;
    std::vector<EventLoop*> loops_;
};


#endif
