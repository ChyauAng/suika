#ifndef _TCPCONTEXT_H_
#define _TCPCONTEXT_H_

#include "base/notCopyable.h"

class Channel;
class EventLoop;
class HttpData;
class Buffer;

class TcpContext:public notCopyable{
public:
    TcpContext(EventLoop* loop);
    ~TcpContext();
    
    void setChannel(int connfd);
    std::shared_ptr<Channel> getChannel(){
        return channel_;
    }

    // callback functions...

    /*
    void shutdown();
    void shutdownInLoop();

    void forceClose();
    void forceCloseInLoop();
     */

private:
    enum StateE{ KConnecting, KConnected, KDisconnecting, KDisconnected};
    void setState(StateE s){
        state_ = s;
    }
    // bool instance in Channel
    
    StateE state_;

    EventLoop* loop;

    // pointer to HttpData
    HttpData* data_;

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    Buffer inputBuffer_;
    Buffer outputBuffer_;

    // socket fd...
    std::shared_ptr<Channel> channel_;
};

#endif
