#include "TimerQueue.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"

#include <functional>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

int createTimerfd();

struct timespec howMuchTimeFromNow(Timestamp when);

void readTimerfd(int timerfd, Timestamp now);

void resetTimerfd(int timerfd, Timestamp expiration);
    

TimerQueue::TimerQueue(EventLoop* loop)
    :loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop, timerfd_),
    timers_(){

    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}


TimerQueue::~TimerQueue(){
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    close(timerfd_);

    for(TimerList::iterator it = timers_.begin(); it != timers_.end(); ++it){
        delete it->second;
    }
}


TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval){
    Timer* timer = new Timer(cb, when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void TimerQueue::addTimerInLoop(Timer* timer){
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);
    
    if(earliestChanged){
        resetTimerfd(timerfd_, timer->expiration());
    }
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now){
    Timestamp nextExpire;

    for(std::vector<Entry>::const_iterator it = expired.begin();
            it != expired.end(); ++it){
        if(it->second->repeat()){
            it->second->restart(now);
        }
        else{
            delete it->second;
        }
    }

    if(!timers_.empty()){
        nextExpire = timers_.begin()->second->expiration();
    }

    if(nextExpire.valid()){
        resetTimerfd(timerfd_, nextExpire);
    }
}

void TimerQueue::handleRead(){
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    // TimerQueue::cancel()
    //callingExpiredTimers_ = true;
    //cancelingTimer_.clear();

    for(std::vector<Entry>::iterator it = expired.begin(); it != expired.end(); ++it){
        it->second->run();
    }
    //callingExpiredTimers_ = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now){
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator end = timers_.lower_bound(sentry);
    // assert(end == timers_.end() || now < end->first);
    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);

    // active timer erase.

    return expired;
}


bool TimerQueue::insert(Timer* timer){
    loop_->assertInLoopThread();
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if(it == timers_.end() || when < it->first){
        earliestChanged = true;
    }
    {
        std::pair<TimerList::iterator, bool>result = timers_.insert(Entry(when, timer));
        assert(result.second);
        (void) result;
    }

    return earliestChanged;
}



int createTimerfd(){
    // CLOCK_MONOTONIC: time after the system starts
    // CLOCK_REALTIME: real time
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    if(timerfd < 0){
        // LOG_SYSDFATAL
    }
    return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when){
    int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    if(microseconds < 100){
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void readTimerfd(int timerfd, Timestamp now){
    uint64_t howmany;
    ssize_t n = read(timerfd, &howmany, sizeof howmany);
    // LOG+_TRACE
    if(n != sizeof howmany){
        // LOG_ERROR
    }
}


void resetTimerfd(int timerfd, Timestamp expiration){
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof newValue);
    bzero(&oldValue, sizeof oldValue);

    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if(ret){
        //LOG_SYSERR
    }
}
