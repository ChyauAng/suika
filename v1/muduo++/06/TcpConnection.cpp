#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Timestamp.h"
#include "SocketsOps.h"
#include "Buffer.h"

#include <unistd.h>
#include <string.h>
// #include <stdio.h>

template<typename To, typename From>
inline To implicit_cast(From const& f){
    return f;
}

TcpConnection::TcpConnection(EventLoop* loop, const string& nameArg, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr)
    :loop_(loop),
    name_(nameArg),
    state_(KConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr){
        channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
        channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
        channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
        channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
        // LOG_DEBUG << "TcpConection::ctor{" << name_ << "] at " << this << " fd= " << sockfd;
        socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection(){
    // LOG_DEBUG
    assert(state_ == KDisconnected);
}


void TcpConnection::sendInLoop(const void* data, size_t len){
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;

    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0){
        nwrote = ::write(channel_->fd(), data, len);
        if(nwrote >= 0){
            if(implicit_cast<size_t>(nwrote) < len){
                // LOG_TRACE << "I am going to write some data.";
            }
        }
        else{
            nwrote = 0;
            if(errno != EWOULDBLOCK){
                // LOG_SYSERR << "TcpConnection::sendInLoop()";
            }
        }
    }
    
    // data still available
    assert(nwrote >= 0);
    if(implicit_cast<size_t>(nwrote) < len){
        outputBuffer_.append(static_cast<const char*>(data) + nwrote, len - nwrote);
        if(!channel_->isWriting()){
            channel_->enableWriting();
        }
    }
}

void TcpConnection::send(const void* data, int len){
    send(std::string(static_cast<const char*>(data), len));
}

void TcpConnection::send(const std::string& message){
    if(state_ == KConnected){
        if(loop_->isInLoopThread()){
            // sendInLoop() first, then handleWrite() by writeCallback
            sendInLoop(message.data(), message.size());
        }
        else{
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message.data(), message.size()));
        }
    }
}

void TcpConnection::shutdown(){
    if(state_ == KConnected){
        setState(KDisconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop(){
    loop_->assertInLoopThread();
    if(!channel_->isWriting()){
        socket_->shutdownWrite();
    }
}

void TcpConnection::handleRead(){
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);

    if(n > 0){
        messageCallback_(shared_from_this(), &inputBuffer_, n);
    }
    else if(n == 0){
        handleClose();
    }
    else{
        errno = savedErrno;
        // LOG_SYSERR << "TcpConnection::handleRead";
        handleError();
    }
}

// after sendInLoop()
void TcpConnection::handleWrite(){
    loop_->assertInLoopThread();
    if(channel_->isWriting()){
        ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
        if(n > 0){
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes() == 0){
                channel_->disableWriting();
                if(state_ == KDisconnecting){
                    shutdownInLoop();
                }
            }
            else{
                // printf("I am going to write some more data.");
                // LOG_TRACE << "I am going to write more data.";
            }
        }
        else{
            // printf("SYSERROR: TcpConnection::handleWrite()");
            // LOG_SYSERR << "TcpConnection::handleWrite()";
        }
    }
    else{
        // printf("Conenction is down");
        // LOG_TRACE << "Connection is down, no more writing".
    }
}

void TcpConnection::handleClose(){
    loop_->assertInLoopThread();
    // LOG_TRACE << "TcpConnection::handleClose state = " << state_
    assert(state_ == KConnected || state_ == KDisconnecting);
    
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
    if(state_ == KConnected){
        setState(KDisconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }

    // the same as loop_->removeChannel(get_pointer(channel_));
    channel_->remove();
}
