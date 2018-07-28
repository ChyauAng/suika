#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_


#include <pthread.h>
#include <memory>
#include <vector>
#include "CurrentThread.h"
#include "notCopyable.h"

 
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

    void updateChannel(Channel* channel);

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
    ChannelList activeChannels_;
};


#endif
