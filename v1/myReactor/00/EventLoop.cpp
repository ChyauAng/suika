#include<poll.h>
#include<unistd.h>
#include<assert.h>

#include"EventLoop.h"

using namespace std;

__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop()
    :looping_(false),
    threadId_(CurrentThread::tid()){
    // do some logging things
    if(t_loopInThisThread){
        // do some logging things
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

    poll(NULL, 0, 5 * 1000);
    
    // do some loging things
    looping_ = false;
}

void EventLoop::abortNotInLoopThread(){
    // do some logging things
    // LOG_FATAL level
}
