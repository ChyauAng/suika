#include "EventLoopThread.h"
#include "EventLoop.h"
#include <functional>
#include <assert.h>
#include <stdio.h>

EventLoopThread::EventLoopThread(): 
    loop_(NULL),
    thread_(std::bind(&EventLoopThread::threadFunc, this)),
    mutex_(),
    cond_(mutex_),
    callback_(NULL){

}

/*
EventLoopThread::EventLoopThread(const ThreadInitCallback& cb): loop_(NULL),
    thread_(std::bind(&EventLoopThread::threadFunc, this)),
    mutex_(),
    cond_(mutex_),
    callback_(cb){

}
*/

EventLoopThread::~EventLoopThread(){
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
    /*
    if(callback_){
        callback_(&loop);
    }
    */
    
    // printf("I am here 1");
    {
        MutexLockGuard lock(mutex_);
        loop_= &loop;
        // printf("I am here 2");
        cond_.notify();
    }

    loop.loop();
    loop_ = NULL;
}

