#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>
#include <memory>

#include "base/notCopyable.h"
#include "base/Timestamp.h"
#include "TcpContext.h"

class EventLoop;

class Channel: public notCopyable{
public:
    typedef std::function<void()> EventCallback;
    
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent();

    void setReadCallback(EventCallback&& cb){
        readCallback_ = cb;
    }
    void setWriteCallback(EventCallback&& cb){
        writeCallback_ = cb;
    }
    void setErrorCallback(EventCallback&& cb){
        errorCallback_ = cb;
    }

    void setCloseCallback(EventCallback&& cb){
        closeCallback_ = cb;
    }
    
    // avoid the owner object being destoryed during handleEvent
    // void tie(const std::shared_ptr<void>&);

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

    void setHolder(std::shared_ptr<TcpContext> holder){
        holder_ = holder;
    }

    std::shared_ptr<TcpContext> getHolder(){
        std::shared_ptr<TcpContext> holder(holder_.lock());
        return holder;
    }

    void setFd(int fd){
        fd_ = fd;
    }


private:
    void update();

    void handleEventWithGuard();
    
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
    
    // bool tied_;
    bool eventHandling_;
    bool addedToLoop_;

    int fd_;
    int events_;
    int revents_;
    // used by Poller
    int index_;

    EventLoop* loop_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;

    // std::weak_ptr<void> tie_;
    // 解决从连接池取出连接后将栈上连接实体(TcpContext)置于哪里的问题
    std::weak_ptr<TcpContext> holder_;
};



#endif
