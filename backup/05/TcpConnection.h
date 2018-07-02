#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_

#include "Callbacks.h"
#include "InetAddress.h"
#include "notCopyable.h"

#include <string>
#include <memory>

class EventLoop;
class Socket;
class Channel;

class TcpConnection: public notCopyable, public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* loop, const std::string& nameArg, int sockfd, const InetAddress & localAddr, const InetAddress& peerAddr);
    ~TcpConnection();
    
    void setConnectionCallback(const ConnectionCallback& cb){
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback& cb){
        messageCallback_ = cb;
    }

    // used by TcpServer and TcpClient
    void setCloseCallback(const CloseCallback& cb){
        closeCallback_ = cb;
    }
    void connectEstablished(); // be called only once by TcpSerevr 
    void connectDestroyed();

    // get function
    EventLoop* getLoop() const{
        return loop_;
    }

    const std::string& name() const {
        return name_;
    }

    const InetAddress& localAddress() const{
        return localAddr_;
    }

    const InetAddress& peerAddress() const{
        return peerAddr_;
    }

    // state function
    bool connected() const{
        return state_ == KConnected;
    }

    bool disconnected() const{
        return state_ == KDisconnected;
    }



private:
    enum StateE { KConnecting, KConnected, KDisconnected};
    void setState(StateE s){
        state_ = s;
    }

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    EventLoop* loop_;
    std::string name_;
    StateE state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
};




#endif
