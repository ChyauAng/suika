#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include "Atomic.h"
#include "TcpConnection.h"
#include "notCopyable.h"

#include <map>
#include <memory>
#include <string>

class Acceptor;
class EventLoop;

class TcpServer: public notCopyable{
public:
    TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& nameArg);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb){
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback& cb){
        messageCallback_ = cb;
    }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    EventLoop* loop_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    AtomicInt32 started_;
    int nextConnId_;
    ConnectionMap connections_;
};

#endif
