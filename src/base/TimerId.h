#ifndef _TIMERID_H_
#define _TIMERID_H_

class Timer;

//user interface
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

