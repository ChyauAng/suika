#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "notCopyable.h"

#include <functional>

class EventLoop;

class Channel: public notCopyable{
public:
    typedef std::function<void()> EventCallback;
    
    Channel(EventLoop* loop, int fd);

    void handleEvent();
    void setReadCallback(const EventCallback& cb){
        readCallback_ = cb;
    }
    void setWriteCallback(const EventCallback& cb){
        writeCallback_ = cb;
    }
    void serErrorCallback(const EventCallback& cb){
        errorCallback_ = cb;
    }

    int fd() const{
        return fd_;
    }

    int events() const{
        return events_;
    }

    void set_revents(int revt){
        revents_ = revt;
    }

    bool isNoneEvent() const{
        return events_ == kNoneEvent;
    }
    
    void enableReading(){
        // 相应位变１
        events_ |= kReadEvent;
        update();
    }
    
    void enableWriting(){
        events_ |= kWriteEvent;
        update();
    }

    void disableWriting(){
        // 相应位变０
        events_ &= ~kWriteEvent;
        update();
    }

    void disableAll(){
        events_ = kNoneEvent;
        update();
    }

    // for Poller
    int index(){
        return index_;
    }

    void setIndex(int idx){
        index_ = idx;
    }

    EventLoop* ownerLoop(){
        return loop_;
    }
    
    // TimerQueue.h
    void remove();

private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    // used by Poller
    int index_;
    bool addedToLoop_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
};



#endif
