#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "base/notCopyable.h"

#include <memory>

class Channel;
class EventLoop;
class EventLoopThreadPool;

class TcpServer:public notCopyable{
public:
    TcpServer(EventLoop* loop, int threadNum, int port);

    ~TcpServer();

    void start();

    void handleNewConn();

private:
    int threadNum_;
    int port_;
    int listeningFd_;

    EventLoop* loop_;

    std::shared_ptr<Channel> acceptChannel_;
    const std::unique_ptr<EventLoopThreadPool> threadPool_;



};













#endif

