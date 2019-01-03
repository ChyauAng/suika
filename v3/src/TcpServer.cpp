#include <netinet/in.h>

#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "HttpData.h"
#include "base/SocketsOps.h"
#include "TcpContext.h"
#include "TcpServer.h"

TcpServer::TcpServer(EventLoop* loop, int threadNum, int port)
    :threadNum_(threadNum),
    port_(port),
    listenFd_(sockets::socketBindListen(port)), //SocketsOps.h
    loop_(loop),
    acceptChannel_(new Channel(loop, listenFd_)),
    threadPool_(new EventLoopThreadPool(loop)){

}

// threadPool_ 中的连接池需要及时清理掉，交给std::list的默认dtor
TcpServer::~TcpServer(){
}

void TcpServer::start(){
    threadPool_->setThreadNum(threadNum_);
    threadPool_->start();
    acceptChannel_->enableReading();  // ET模式的话需要在Channel的事件中更改(KReadEvent与KWriteEvent)
    acceptChannel_->setReadCallback(std::bind(&TcpServer::handleNewConn, this));
    loop_->updateChannel(acceptChannel_);
}

void TcpServer::handleNewConn(){
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof addr);
    int connfd = 0;
    while((connfd = sockets::accept(listenFd_, &addr)) > 0){
        EventLoop* ioLoop = threadPool_->getNextLoop();

        if(sockets::setSocketNonBlocking(connfd) < 0){
            return;
        }
        sockets::setTcpNoDelay(connfd, true);
        sockets::setReuseAddr(connfd, true);
        sockets::setReusePort(connfd, true);
        ioLoop->runInLoop(std::bind(&EventLoop::getFreeContext, ioLoop, connfd));
    }
    // 到目前为止仍为LT模式，若为ET模式，应添加
    // acceptChannel_->enableReading();
}
