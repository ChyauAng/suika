#include <assert.h>
#include <sys/epoll.h>

#include "Channel.h"
#include "EventLoop.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    :eventHandling_(false),
    addedToLoop_(false),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1),
    loop_(loop){

}

Channel::~Channel(){
    assert(!eventHandling_);
}

void Channel::update(){
    addedToLoop_ = true;
    loop_->updateChannel(this);
}

void Channel::remove(){
    assert(isNoneEvent());
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::handleEvent(){
    handleEventWithGuard();
}

void Channel::handleEventWithGuard(){
    eventHandling_ = true;
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
        if(closeCallback_){
            closeCallback_();
        }
    }
    if(revents_ & (EPOLLERR)){
        if(errorCallback_){
            errorCallback_();
        }
    }
    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
        if(readCallback_){
            readCallback_();
        }
    }
    if(revents_ & (EPOLLOUT)){
        if(writeCallback_){
            writeCallback_();
        }
    }
    eventHandling_ = false;
}

