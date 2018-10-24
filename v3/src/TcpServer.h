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
    int listenFd_;

    EventLoop* loop_;

    Channel* acceptChannel_;
    const std::unique_ptr<EventLoopThreadPool> threadPool_;



};













#endif

