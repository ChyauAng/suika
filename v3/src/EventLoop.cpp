#include <assert.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <stdio.h>

#include "Channel.h"
#include "EPollPoller.h"
#include "EventLoop.h"
#include "TcpContext.h"

using namespace std;

__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;
const int MaxConnectionPoolSize = 512;

int createEventfd();

// 为保证线程安全，跨线程对象在构造函数期间不可把this指针泄露出去
// EventLoop不为跨线程对象，它与IO线程是一一对应关系
EventLoop::EventLoop()
    :looping_(false),
    eventHandling_(false),
    quit_(false),
    callingPendingFunctors_(false),
    wakeupFd_(createEventfd()),
    threadId_(CurrentThread::tid()),
    wakeupChannel_(new Channel(this, wakeupFd_)),
    currentActiveChannel_(NULL),
    poller_(new EPollPoller(this)),
    timerQueue_(new TimerQueue(this)),
    freeContext_(NULL){
    // LOG_DEBUG << "init EventLoop";
    if(t_loopInThisThread){
        // LOG_FATAL << "another thread can access this eventloop";
     }
    else{
        t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));  // wake up the possibly blocked IO thread
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop(){
    assert(!looping_);
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::initContextPool(){
    freeContext_ = std::shared_ptr<TcpContext>(new TcpContext(this));
    std::shared_ptr<TcpContext> tc(freeContext_);
    for(int i = 0; i < MaxConnectionPoolSize; i++){
        std::shared_ptr<TcpContext> tct(new TcpContext(this));
        tct->setIndex(i);
        tc->setTData(tct);
        tc = tc->getTData();
    }
}

void EventLoop::loop(){
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    while(!quit_){
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);

        eventHandling_ = true;
        for(ChannelList::iterator it = activeChannels_.begin(); it != activeChannels_.end(); ++it){
            currentActiveChannel_ = (*it);
            currentActiveChannel_->handleEvent();
        }
        currentActiveChannel_= NULL;
        eventHandling_ = false;
        doPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::quit(){
    quit_ = true;
     if(!isInLoopThread()){
         wakeup();
     }
}

TimerId EventLoop::runAt(const Timestamp& time, TimerCallback&& cb){
    return timerQueue_->addTimer(std::move(cb), time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback&& cb){
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallback&& cb){
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(std::move(cb), time, interval);
}

void EventLoop::runInLoop(Functor&& cb){
    if(isInLoopThread()){
        cb();
    }
    else{
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor&& cb){
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(std::move(cb));
    }
    if(!isInLoopThread() || callingPendingFunctors_){
        wakeup();
    }
}
void EventLoop::wakeup(){
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
    assert(n == one);
    if(n != sizeof one){
        // LOG_ERROR << "failed to wake up eventloop";
    }
}

void EventLoop::handleRead(){
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one)){
        // LOG_ERROR << "failed to handle read event of eventloop";
    }
}

void EventLoop::doPendingFunctors(){
    std::vector<Functor> functors;  // narrow the critical area
    callingPendingFunctors_ = true;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(size_t i = 0; i < functors.size(); i++){
        functors[i]();
    }
    callingPendingFunctors_ = false;
}


void EventLoop::updateChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    // TODO: take this channel being active channel or not into consideration
    if(eventHandling_){
        assert(currentActiveChannel_ == channel || std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    }
    poller_->removeChannel(channel);
}


void EventLoop::abortNotInLoopThread(){
    // LOG_FATAL << "access this eventloop in wrong thread";
}

int createEventfd(){
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0){
        // LOG_SYSERR << "cannot get right event fd";
        abort();
    }
    return evtfd;
}
