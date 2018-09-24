#include <assert.h>
#include <sys/epoll.h>

#include "Channel.h"
#include "EventLoop.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    // :tied_(false), 
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
    /*
    std::shared_ptr<void> guard;
    if(tied_){
        // weak_ptr --> shared_ptr
        guard = tie_.lock();
        if(guard){
            handleEventWithGuard();
        }
    }
    else{
        handleEventWithGuard();
    }
    */
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
            // printf("read channel fd is %d\n", fd_);
            // printf("read holder use count is%d\n", holder_.use_count());
            // printf("The current read tid is %d\n", CurrentThread::tid());
            // printf("********************\n");

            readCallback_();
        }
    }

    if(revents_ & (EPOLLOUT)){
        if(writeCallback_){
            // printf("write channel fd is %d\n", fd_);
            // printf("write holder use count is%d\n", holder_.use_count());
            // printf("The current write id is %d\n", CurrentThread::tid());
            writeCallback_();
        }
    }
    eventHandling_ = false;
}
/*
void Channel::tie(const std::shared_ptr<void>& obj){
    tie_ = obj;
    tied_ = true;
}
*/

