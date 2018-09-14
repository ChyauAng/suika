#include <netinet/in.h>

#include "Channel.h"
#include "EventLoopThread.h"
#include "base/SocketsOps.h"
#include "TcpContext.h"
#include "TcpServer.h"

TcpServer::TcpServer(EventLoop* loop, int ThreadNum, int port)
    :threadNum_(threadNum),
    port_(port),
    listenFd_(socketBindListen(port)) //SocketsOps.h
    loop_(loop),
    acceptChannel_(new Channel(loop, listenFd_)),
    threadPool_(new EventLoopTHreadPool(loop)){
        if(setSocketNonBlocking(listenFd_) < 0){  // SocketsOps.h
            perror("set socket fd nonblocking failed.");
            abort();
        }
    }

TcpServer::~TcpServer(){
// threadPool_ 中的连接池需要及时清理掉，交给std::list的默认dtor
}

void TcpServer::start(){
    threadPool_->start();
    // ET模式的话需要在Channel的事件中更改(KReadEvent与KWriteEvent)
    acceptChannel_->enableReading();
    acceptChannel_->setReadCallback(std::bind(&Server::handleNewConn, this));
    loop_->updateChannel(acceptChannel_);
}

void TcpServer::handleNewConn(){
    // Accptor部分内容
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof addr);
    int connfd = 0;
    while((connfd = sockets::accept(listenFd_, &addr)) > 0){
        EventLoop* ioLoop = threadPool_->getNextLoop();

        if(setSocketNonBlocking(connfd) < 0){
            return;
        }

        sockets::setTcpNoDelay(connfd, true);
        sockets::setReuseAddr(connfd, true);
        sockets::setReusePort(connfd, true);
        
        // ioLoop获得空闲TcpContext
        TcpContext tcpConetxt(ioLoop->getFreeContext());
        tcpContext->setState(KConnected);
        // 将connfd传入TcpContext，生成Channel，通过ioLoop->updateChannel注册到ioLoop的poller_中
        // 通过调用TcpContext中某函数ioLoop->queueInLoop(&TcpContext::newEvent, this),(X) 不需要，直接将事件绑定至Channel即可
        tcpContext->setChannel(connfd);
        tcpContext->getChannel()->setHolder(tcpContext);
        ioLoop->updateChannel(tcpContext->getChannel());
        
        // tcpConetxt->addInLoop();

    }
    // 到目前为止仍为LT模式，若为ET模式，应添加
    // acceptChannel_->enableReading();

}
