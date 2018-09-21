#include "EventLoopThread.h"
#include "EventLoop.h"
#include <functional>
#include <assert.h>
#include <stdio.h>

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb)     
    :existing_(false),
    loop_(NULL),
    callback_(cb),
    mutex_(),
    cond_(mutex_),
    thread_(std::bind(&EventLoopThread::threadFunc, this)){

}

EventLoopThread::~EventLoopThread(){
    existing_ = true;
    if(loop_ != NULL){
        // not 100% thread safe
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop(){
    assert(!thread_.started());
    thread_.start();

    {
        MutexLockGuard lock(mutex_);
        while(loop_ == NULL){
            cond_.wait();

        }
    }

    return loop_;
}

void EventLoopThread::threadFunc(){
    EventLoop loop;
    
    if(callback_){
        callback_(&loop);
    }
    
    {
        MutexLockGuard lock(mutex_);
        loop_= &loop;
        cond_.notify();
    }

    loop.initContextPool();
    // printf("Context poll init completed\n");
    loop.loop();

    loop_ = NULL;
}

