#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketsOps.h"
#include "Callbacks.h"

#include <stdio.h>

void defaultConnectionCallback(const TcpConnectionPtr& conn){
    // do some log things
}

void defaultMessageCallback(const TcpConnectionPtr& conn, const char* buf, int n){
    //
}

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& nameArg)
    :loop_(loop),
    name_(nameArg),
    acceptor_(new Acceptor(loop, listenAddr, 1)),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback),
    nextConnId_(1){
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer(){
    loop_->assertInLoopThread();
    // LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";

    for(ConnectionMap::iterator it(connections_.begin()); it != connections_.end(); ++it){
        TcpConnectionPtr conn(it->second);
        it->second.reset();
        // conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::start(){
    if(started_.getAndSet(1) == 0){
        assert(!acceptor_->listenning());
        loop_->runInLoop(std::bind(&Acceptor::listen, get_pointer(acceptor_)));
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr){
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;
    string connName = name_ + buf;

    // LOG_INFO << "TcpServer::newConnection [" << name_
    //          << "] - new connection [" << connName
    //          << "] from " << peerAddr.toIpPort();

    InetAddress localAddr(sockets::getLocalAddr(sockfd));

    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));

    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn){
    loop_->assertInLoopThread();
    // LOG_INFO << "TcpServer::removeConnection [" << name_ << "] - connection " << conn->name();
    size_t n = connections_.erase(conn->name());
    assert(n == 1);
    (void) n;
    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}
