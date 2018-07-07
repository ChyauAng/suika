#ifndef _EVENTLOOPTHREADPOOL_H_
#define _EVENTLOOPTHREADPOOL_H_

#include "notCopyable.h"

#include <vector>
#include <functional>
#include <string>

class EventLoop;
class EventLoopThread;
class EventLoopThreadPool;

// stack variable
class EventLoopThreadPool: public notCopyable{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;
    EventLoopThreadPool(EventLoop* loop, const std::string& nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads){
        numThreads_ = numThreads;
    }

    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    EventLoop* getNextLoop();
    EventLoop* getLoopForHash(size_t hashCode);
    std::vector<EventLoop*> getAllLoops();

    bool started() const{
        return started_;
    }

    const std::string& name() const{
        return name_;
    }

private:
    EventLoop* baseLoop_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<EventLoopThread*> threads_;
    std::vector<EventLoop*> loops_;
};


#endif
