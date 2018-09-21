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

    // schedules the callback to be run at given time
    TimerId addTimer(TimerCallback&& cb, Timestamp when, double interval);


    void cancel(TimerId timerId);

private:
    // binary search tree
    // other options: linear list or priority queue based on binary heap(LinYaCool::Websever)
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);
    // handle the readable timefd event
    void handleRead();

    // get the expired timer
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);
    bool insert(Timer* timer);


    bool callingExpiredTimers_;

    const int timerfd_;

    EventLoop* loop_;

    std::shared_ptr<Channel> timerfdChannel_;

    // timers sorted by expiration
    TimerList timers_;
    // for calcel()
    // timers sorted by object addresses
    ActiveTimerSet activeTimers_;
    // avoid self-unregister
    ActiveTimerSet cancelingTimers_;


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
