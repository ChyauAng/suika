#include <poll.h>
#include "EventLoop.h"
#include "Channel.h"
#include <assert.h>
// #include <stdio.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN |POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    :loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1),
    addedToLoop_(false),
    tied_(false), 
    eventHandling_(false){

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

void Channel::handleEvent(Timestamp t){
    std::shared_ptr<void> guard;
    if(tied_){
        // weak_ptr --> shared_ptr
        guard = tie_.lock();
        if(guard){
            handleEventWithGuard(t);
        }
    }
    else{
        handleEventWithGuard(t);
    }
}

void Channel::handleEventWithGuard(Timestamp t){
    eventHandling_ = true;

    // printf("I am here: Channel::handleEventWithGuard");

    if((revents_ & POLLHUP) && !(revents_ & POLLIN)){
        if(closeCallback_){
            // printf("I am here: Channel::handleEventWithGuard--closeCallback_()\n");
            closeCallback_();
        }
    }

    if(revents_ & POLLNVAL){
        // LOG_WARN
        // printf("I am here: Channel::handleEventWithGuard--L66()\n");
    }

    if(revents_ & (POLLERR | POLLNVAL)){
        if(errorCallback_){
            // printf("I am here: Channel::handleEventWithGuard--errorCallback_()\n");
            errorCallback_();
        }
    }

    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP)){
        if(readCallback_){
            // printf("I am here: Channel::handleEventWithGuard--readCallback_()\n");
            readCallback_(t);
        }
    }

    if(revents_ & (POLLOUT)){
        if(writeCallback_){
            // printf("I am here: Channel::handleEventWithGuard--wirteCallback_()\n");
            writeCallback_();
        }
    }
    eventHandling_ = false;
}

void Channel::tie(const std::shared_ptr<void>& obj){
    tie_ = obj;
    tied_ = true;
}

