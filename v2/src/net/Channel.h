#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "notCopyable.h"
#include "Timestamp.h"

#include <functional>
#include <memory>

class EventLoop;

class Channel: public notCopyable{
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(Timestamp)> ReadEventCallback;
    
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(Timestamp t);

    void setReadCallback(ReadEventCallback&& cb){
        readCallback_ = std::move(cb);
    }
    void setWriteCallback(EventCallback&& cb){
        writeCallback_ = std::move(cb);
    }
    void setErrorCallback(EventCallback&& cb){
        errorCallback_ = std::move(cb);
    }

    void setCloseCallback(EventCallback&& cb){
        closeCallback_ = std::move(cb);
    }
    
    // avoid the owner object being destoryed during handleEvent
    void tie(const std::shared_ptr<void>&);

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

    bool isReading() const{
        return events_ & kReadEvent;
    }

    bool isWriting() const{
        return events_ & kWriteEvent;
    }


private:
    void update();

    void handleEventWithGuard(Timestamp t);
    
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
    
    std::weak_ptr<void> tie_;
    bool tied_;
    bool eventHandling_;

    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    // used by Poller
    int index_;
    bool addedToLoop_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};



#endif
