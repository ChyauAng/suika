#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_


#include <pthread.h>
#include <memory>
#include <vector>
#include "CurrentThread.h"
#include "notCopyable.h"
#include "Timestamp.h"
#include "TimerId.h"
#include "Callbacks.h"
#include "TimerQueue.h"

 
class Poller;
class Channel;

class EventLoop : public notCopyable{
public:
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


    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    bool isInLoopThread() const{
        return threadId_ == CurrentThread::tid();
    }

private:
    void abortNotInLoopThread();

    typedef std::vector<Channel*> ChannelList;
    
    bool looping_;
    bool quit_;

    const pid_t threadId_;

    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;
    ChannelList activeChannels_;
};


#endif
