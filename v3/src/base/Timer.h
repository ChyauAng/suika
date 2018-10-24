#ifndef _TIMER_H_
#define _TIMER_H_

#include "Callbacks.h"
#include "notCopyable.h"
#include "Timestamp.h"


class Timer: public notCopyable{
public:
    Timer(TimerCallback&& cb, Timestamp when, double interval)
        :repeat_(interval > 0.0),
         callback_(std::move(cb)),
         expiration_(when),
         interval_(interval),
         sequence_(__sync_fetch_and_add(&s_numCreated_, 1) + 1){

    }
    
    // callback
    void run() const{
        callback_();
    }
    
    // get functions
    Timestamp expiration() const{
        return expiration_;
    }
    bool repeat() const{
        return repeat_;
    }
    int64_t sequence() const{
        return sequence_;
    } 
    static int64_t numCreated(){
        return __sync_val_compare_and_swap(&s_numCreated_, 0, 0);
    }
    

    void restart(Timestamp now);


private:
    const bool repeat_;

    const int sequence_;

    const TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;
    // identity the object

    static int s_numCreated_;
};

#endif

