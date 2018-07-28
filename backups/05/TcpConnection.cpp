#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Timestamp.h"
#include "SocketsOps.h"

#include <unistd.h>

TcpConnection::TcpConnection(EventLoop* loop, const string& nameArg, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr)
    :loop_(loop),
    name_(nameArg),
    state_(KConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr){
        channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
        // LOG_DEBUG << "TcpConection::ctor{" << name_ << "] at " << this << " fd= " << sockfd;
        socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection(){
    // LOG_DEBUG
    assert(state_ == KDisconnected);
}


void TcpConnection::handleRead(){
    char buf[65536];
    ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
    if(n > 0){
        messageCallback_(shared_from_this(), buf, n);
    }
    else if(n == 0){
        handleClose();
    }
    else{
        handleError();
    }
}

void TcpConnection::handleClose(){
    loop_->assertInLoopThread();
    // LOG_TRACE << "TcpConnection::handleClose state = " << state_
    assert(state_ == KConnected);
    
    setState(KDisconnected);

    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError(){
    int err = sockets::getSocketError(channel_->fd());
    // LOG_ERROR << "TcpConnection::handleError [" << name_ << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void TcpConnection::connectEstablished(){
    loop_->assertInLoopThread();
    assert(state_ == KConnecting);
    setState(KConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed(){
    loop_->assertInLoopThread();
    assert(state_ == KConnected);
    setState(KDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());

    loop_->removeChannel(get_pointer(channel_));
}
