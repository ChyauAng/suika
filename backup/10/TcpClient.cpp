#include "TcpClient.h"
#include "Connector.h"
#include "EventLoop.h"
#include "SocketsOps.h"

#include <stdio.h>

void removeConnectionNoMF(EventLoop* loop, const TcpConnectionPtr& conn){
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void removeConnector(const ConnectorPtr& connector){

}

TcpClient::TcpClient(EventLoop* loop, const InetAddress& servAddr, const std::string& nameArg)
    :loop_(loop),
    connector_(new Connector(loop, servAddr)),
    name_(nameArg),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback),
    retry_(false),
    connect_(true),
    nextConnId_(1){
        connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
        // LOG_INFO << "TcpClient::TcpClient[" << name_<<"] - connector " << get_pointer(connector_);
}

TcpClient::~TcpClient(){
    // LOG_INFO << "TcpClient::~TcpClient[" << name_ << "] - connetor " << get_pointer(connector_);

    TcpConnectionPtr conn;
    bool unique = false;
    {
        MutexLockGuard lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if(conn){
        assert(loop_ == conn->getLoop());
        CloseCallback cb = std::bind(&removeConnectionNoMF, loop_, std::placeholders::_1);
        loop_->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if(unique){
            conn->forceClose();
        }
    }
    else{
        connector_->stop();
        loop_->runAfter(1, std::bind(&removeConnector, connector_));
    }
}

void TcpClient::connect(){
    // LOG_INFO << "TcpClient::connect[" << name_ << "] - connecting to " << connector_->servAddress().toIpPort();
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect(){
    connect_ = false;
    {
        MutexLockGuard lock(mutex_);
        if(connection_){
            connection_->shutdown();
        }
    }
}

void TcpClient::stop(){
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd){
    loop_->assertInLoopThread();
    InetAddress peerAddress(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddress.toIpPort().c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddress));
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));

    // unsafe
    {
        MutexLockGuard lock(mutex_);
        connection_ = conn;
    }
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn){
    loop_->assertInLoopThread();
    assert(loop_ == conn->getLoop());

    {
        MutexLockGuard lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if(retry_ && connect_){
        // LOG_INFO << "TcpClient::connect[" << name_ << "] - Reconnecting to" << connector_->servAddress().toIpPort();
        connector_->restart();
    }
}
