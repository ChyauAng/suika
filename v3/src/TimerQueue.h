#ifndef _TIMERQUEUE_H_
#define _TIMERQUEUE_H_

#include <memory>
#include <set>
#include <sys/timerfd.h>
#include <vector>

#include "base/Callbacks.h"
#include "Channel.h"
#include "base/notCopyable.h"
#include "base/Timestamp.h"

class EventLoop;
class Timer;
class TimerId;

class TimerQueue: public notCopyable{
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(TimerCallback&& cb, Timestamp when, double interval);  // schedules the callback to be run at given time

    void cancel(TimerId timerId);

private:
    typedef std::pair<Timestamp, Timer*> Entry;  // BST or other options: linear list or priority queue based on binary heap(LinYaCool::Websever)
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);
    void handleRead();  // handle the readable timefd event

    std::vector<Entry> getExpired(Timestamp now);  // get the expired timer
    void reset(const std::vector<Entry>& expired, Timestamp now);
    bool insert(Timer* timer);

    bool callingExpiredTimers_;

    const int timerfd_;

    EventLoop* loop_;

    std::shared_ptr<Channel> timerfdChannel_;

    TimerList timers_;  // timers sorted by expiration
    
    ActiveTimerSet activeTimers_;  // for calcel() and timers sorted by object addresses
    ActiveTimerSet cancelingTimers_;  // avoid self-unregister
};


class TimerId{
public:
    TimerId()
        :sequence_(0),
        timer_(NULL){
    }

    TimerId(Timer* timer, int seq)
        :sequence_(seq),
        timer_(timer){
    }

    friend class TimerQueue;

private:
    int sequence_;
    Timer* timer_;
};

#endif
