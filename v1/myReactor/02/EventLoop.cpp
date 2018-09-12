#include <poll.h>
#include <unistd.h>
#include <assert.h>

#include "Poller.h"
#include "Channel.h"
#include "EventLoop.h"

using namespace std;

__thread EventLoop* t_loopInThisThread = 0;
const int kPollTimeMs = 10000;

// 为保证线程安全，跨线程对象在构造函数期间不可把this指针泄露出去
// EventLoop不为跨线程对象，它与IO线程是一一对应关系
EventLoop::EventLoop()
    :looping_(false),
    quit_(false),
    poller_(new Poller(this)),
    threadId_(CurrentThread::tid()),
    timerQueue_(new TimerQueue(this)){
    // do some logging things
    // LOG_DEBUG
    if(t_loopInThisThread){
        // do some logging things
        // LOG_FATAL: another thread can access this eventloop
     }
    else{
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop(){
    assert(!looping_);
    t_loopInThisThread = NULL;
}

void EventLoop::loop(){
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;

    while(!quit_){
        activeChannels_.clear();
        poller_->polll(kPollTimeMs, &activeChannels_);

        for(ChannelList::iterator it = activeChannels_.begin(); it != activeChannels_.end(); ++it){
            (*it)->handleEvent();
        }
    }

    // LOG_TRACE
    looping_ = false;
}

void EventLoop::quit(){
    quit_ = true;
    // wake up
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb){
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb){
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb){
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::updateChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    
    // to do: take this channel being active channel or not into consideration

    poller_->removeChannel(channel);
}


void EventLoop::abortNotInLoopThread(){
    // do some logging things
    // LOG_FATAL level
}
