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
#include "TcpContext.h"
#include "TimerQueue.h"
#include "base/Timestamp.h"

 
class Channel;
class EPollPoller;

class EventLoop : public notCopyable{
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();

    void initContextPool();

    void loop();

    void assertInLoopThread(){
        if(!isInLoopThread()){
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
    // void runInLoop(InitFunctor&& cb);
    // add the callback function into the task queue of the current thread.
    void queueInLoop(Functor&& cb);
    
    void wakeup();  // internal usage

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    
    // 涉及构造与数据拷贝，需要改进
    // 考虑使用一种容器，可插入／获取删除(std::list貌似不支持，考虑自己实现list)
    std::shared_ptr<TcpContext> getFreeContext(int connfd){
        std::shared_ptr<TcpContext> tc;
        if(freeContext_->getTData() != NULL){
            tc = freeContext_->getTData();
            freeContext_->setTData(tc->getTData());
            tc->setTDataNull();
        }
        else{
            tc = std::shared_ptr<TcpContext>(new TcpContext(this));

        }
        activeContexts_.push_back(tc);
        tc->setStateKConnected();
        tc->setChannel(connfd);
        tc->configEvent();
        return tc;
    }

    void givebackContext(std::shared_ptr<TcpContext> stc){
        stc->setTData(freeContext_->getTData());
        freeContext_->setTData(stc);
        TcpContextList::iterator i = std::find(activeContexts_.begin(), activeContexts_.end(), stc);
        assert(i != activeContexts_.end());
        activeContexts_.erase(i);
    }

private:
    typedef std::vector<Channel*> ChannelList;  // Channel in interest
    typedef std::vector<std::shared_ptr<TcpContext>> TcpContextList;

    void abortNotInLoopThread();
    void handleRead();  // wakeupFd_:eventfd, wakeup()
    void doPendingFunctors();  // todo task in task queue

    bool looping_;
    bool eventHandling_;
    bool quit_;
    bool callingPendingFunctors_;

    int wakeupFd_;  // Fuctors in doPendingFunctors() may call queueInLoop(functor) again, so queueInLoop() should wake up the IO thread.
    const pid_t threadId_;

    Channel* wakeupChannel_;
    Channel* currentActiveChannel_;
    
    const std::unique_ptr<EPollPoller> poller_;  // small functors optimization
    const std::unique_ptr<TimerQueue> timerQueue_;

    std::shared_ptr<TcpContext> freeContext_;  // 连接对象池

    std::vector<Functor> pendingFunctors_;

    ChannelList activeChannels_;
    TcpContextList activeContexts_;

    MutexLock mutex_;
};

#endif
