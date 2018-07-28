#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_


#include <pthread.h>
#include <memory>
#include <vector>
#include <functional>
#include "CurrentThread.h"
#include "notCopyable.h"
#include "Timestamp.h"
#include "TimerId.h"
#include "Callbacks.h"
#include "TimerQueue.h"
#include "Mutex.h"

 
class EPollPoller;
class Channel;

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

    void quit();

    TimerId runAt(const Timestamp& time, const TimerCallback& cb);
    TimerId runAfter(double delay,const TimerCallback& cb);
    TimerId runEvery(double interval, const TimerCallback& cb);
    
    // schedules the task during different threads.
    // run the callback fuction when in the current IO thread, otherwise add into the task queue of the thread.
    void runInLoop(const Functor& cb);
    // add the callback function into the task queue of the current thread.
    void queueInLoop(const Functor& cb);
    
    // internal usage
    void wakeup();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    bool isInLoopThread() const{
        return threadId_ == CurrentThread::tid();
    }

private:
    void abortNotInLoopThread();
    // eventfd
    void handleRead();
    // todo task in task queue
    void doPendingFunctors();

    typedef std::vector<Channel*> ChannelList;
    
    bool looping_;
    bool quit_;
    // Fuctors in doPendingFunctors() may call queueInLoop(functor) again, so queueInLoop() should wake up the IO thread.
    bool callingPendingFunctors_;
    // wakeup fd:wakeup(), handleRead()
    int wakeupFd_;

    const pid_t threadId_;


    std::unique_ptr<EPollPoller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;
    ChannelList activeChannels_;
    Channel* currentActiveChannel_;
    bool eventHandling_;
    // wakeupFd
    std::unique_ptr<Channel> wakeupChannel_;
    // doPendingFunctors
    std::vector<Functor> pendingFunctors_;

    MutexLock mutex_;
};


#endif
