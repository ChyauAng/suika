#ifndef _TIMERID_H_
#define _TIMERID_H_

#include <stdint.h>

class Timer;

//user interface
class TimerId{
public:
    TimerId()
        :timer_(NULL),
        sequence_(0){
        
    }

    TimerId(Timer* timer, int64_t seq)
        :timer_(timer),
        sequence_(seq){
   
    }

    friend class TimerQueue;

private:
    Timer* timer_;
    int64_t sequence_;
};

#endif

