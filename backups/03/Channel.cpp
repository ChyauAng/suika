#include <poll.h>
#include "EventLoop.h"
#include "Channel.h"
#include <assert.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN |POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    :loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1),
    addedToLoop_(false){

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
    if(revents_ & POLLNVAL){
        // LOG_WARN
    }

    if(revents_ & (POLLERR | POLLNVAL)){
        if(errorCallback_){
            errorCallback_();
        }
    }

    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP)){
        if(readCallback_){
            readCallback_();
        }
    }

    if(revents_ & (POLLOUT)){
        if(writeCallback_){
            writeCallback_();
        }
    }
}

