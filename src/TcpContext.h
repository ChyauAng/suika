#ifndef _TCPCONTEXT_H_
#define _TCPCONTEXT_H_

#include <boost/any.hpp>
#include <functional>
#include <memory>

#include "base/Buffer.h"
#include "base/notCopyable.h"

class Channel;
class EventLoop;
class HttpData;

class TcpContext:public notCopyable, public std::enable_shared_from_this<TcpContext>{
public:
    typedef std::function<void(Buffer*)> Callback;
    TcpContext(EventLoop* loop);
    ~TcpContext();
    
    void setChannel(int connfd);
    std::shared_ptr<Channel> getChannel(){
        return channel_;
    }
    
    std::shared_ptr<TcpContext> getTData(){
        return tdata_;
    }
    void setTData(std::shared_ptr<TcpContext> data){
        tdata_ = data;
    }
    void setTDataNull(){
        tdata_.reset();
    }

    void setRequestCallback(const Callback& cb){
        requestCallback_ = std::move(cb);
    }

    // callback functions...

    /*
    void shutdown();
    void shutdownInLoop();

    void forceClose();
    void forceCloseInLoop();
     */
    void shutdown();
    void shutdownInLoop();


    void setStateKConnected(){
        state_ = KConnected;
    }

    Buffer& getInputBuffer(){
        return inputBuffer_;
    }
    Buffer& getOutputBuffer(){
        return outputBuffer_;
    }

    void send(Buffer* buf);

    void configEvent();

    void setIndex(int index){
        index_ = index;
    }

    int getIndex(){
        return index_;
    }

    void reset(){
        state_ = KConnecting;
        inputBuffer_.retrieveAll();
        outputBuffer_.retrieveAll();
    }
private:
    void sendInLoop(const char* message, size_t len);

    enum StateE{ KConnecting, KConnected, KDisconnecting, KDisconnected};
    void setState(StateE s){
        state_ = s;
    }

    // bool instance in Channel
    
    StateE state_;
    int index_;

    EventLoop* loop_;


    Callback requestCallback_;

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    // 当在连接池中时，作为指针指向下一个空闲连接
    std::shared_ptr<TcpContext> tdata_;
    // 从连接池中取出时，作为指针指向数据
    std::shared_ptr<HttpData> hdata_;
    // socket fd...
    std::shared_ptr<Channel> channel_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;

};

#endif
