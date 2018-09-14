#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_


#include <functional>
#include <memory>
#include <pthread.h>
#include <vector>

#include "base/Callbacks.h"
#include "base/CurrentThread.h"
#include "base/Mutex.h"
#include "base/notCopyable.h"
#include "TimerQueue.h"
#include "base/Timestamp.h"

 
class Channel;
class EPollPoller;
class TcpContext;

class EventLoop : public notCopyable{
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();

    void loop();

    void assertInLoopThread(){
        if(!isInLoopThread()){
            // do some logging things
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const{
        return threadId_ == CurrentThread::tid();
    }

    void quit();
    
    TimerId runAt(const Timestamp& time, TimerCallback&& cb);
    TimerId runAfter(double delay, TimerCallback&& cb);
    TimerId runEvery(double interval, TimerCallback&& cb);

    // schedules the task during different threads.
    // run the callback fuction when in the current IO thread, otherwise add into the task queue of the thread.
    void runInLoop(Functor&& cb);
    // add the callback function into the task queue of the current thread.
    void queueInLoop(Functor&& cb);
    
    // internal usage
    void wakeup();

    void updateChannel(std::shared_ptr<Channel> channel);
    void removeChannel(std::shared_ptr<Channel> channel);
    
    // 涉及构造与数据拷贝，需要改进
    // 考虑使用一种容器，可插入／获取删除(std::list貌似不支持，考虑自己实现list)
    TcpContext getFreeContext(){
        TcpContext tc(tcpContext_.at(freeConnectionIndex_));
        tcpContext_.erase(freeConnectionIndex_);
        return tc;
    }

    void givebackContext(TcpContext tc){
        tcpContext_.insert(freeConnectionIndex_);
        // STL标准：插入将插入至位置之前，所以索引值刚好指在归还的连接实体上
        // freeConnectionIndex_ -= 1;
    }



private:
    // Channel in interest
    typedef std::vector<std::shared_ptr<Channel>> ChannelList;

    void abortNotInLoopThread();
    // eventfd, wakeup()
    void handleRead();
    // todo task in task queue
    void doPendingFunctors();

    
    bool looping_;
    bool eventHandling_;
    bool quit_;
    // Fuctors in doPendingFunctors() may call queueInLoop(functor) again, so queueInLoop() should wake up the IO thread.
    bool callingPendingFunctors_;

    // wakeup fd:wakeup(), handleRead()
    int wakeupFd_;
    const pid_t threadId_;
    int freeConnectionIndex_;
    
    Channel* currentActiveChannel_;
    // small functors optimization
    const std::unique_ptr<EPollPoller> poller_;
    const std::unique_ptr<TimerQueue> timerQueue_;
    // wakeupFd
    const std::unique_ptr<Channel> wakeupChannel_;
    // doPendingFunctors
    std::vector<Functor> pendingFunctors_;
    
    // 连接池
    std::vector<TcpContext> tcpContext_;

    ChannelList activeChannels_;

    MutexLock mutex_;
};


#endif
