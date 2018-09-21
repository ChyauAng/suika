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

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    
    // 涉及构造与数据拷贝，需要改进
    // 考虑使用一种容器，可插入／获取删除(std::list貌似不支持，考虑自己实现list)
    std::shared_ptr<TcpContext> getFreeContext(){
        std::shared_ptr<TcpContext> tc(freeContext_->getTData());
        freeContext_->setTData(tc->getTData());
        tc->setTDataNull();
        activeContexts_.push_back(tc);
        printf("1 use count is %d\n", tc.use_count());
        return tc;
    }

    void givebackContext(std::shared_ptr<TcpContext> stc){
        printf("2 use count is %d\n", stc.use_count());
        // stc->setHDataNull();
        stc->setTData(freeContext_->getTData());
        freeContext_->setTData(stc);
        TcpContextList::iterator i = std::find(activeContexts_.begin(), activeContexts_.end(), stc);
        assert(i != activeContexts_.end());
        activeContexts_.erase(i);
        printf("3 use count is %d\n", stc.use_count());
    }


private:
    // Channel in interest
    typedef std::vector<Channel*> ChannelList;
    typedef std::vector<std::shared_ptr<TcpContext>> TcpContextList;

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

    Channel* wakeupChannel_;
    Channel* currentActiveChannel_;
    
    // small functors optimization
    const std::unique_ptr<EPollPoller> poller_;
    const std::unique_ptr<TimerQueue> timerQueue_;
    // wakeupFd

    // 空闲链表头
    // std::shared_ptr<TcpContext> tcpContext_;
    // 指向空闲TcpContext前一个
    std::shared_ptr<TcpContext> freeContext_;

    // doPendingFunctors
    std::vector<Functor> pendingFunctors_;
    
    // 连接池

    ChannelList activeChannels_;
    TcpContextList activeContexts_;

    MutexLock mutex_;

};


#endif
