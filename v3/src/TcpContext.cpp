#include "base/Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "HttpData.h"
#include "base/SocketsOps.h"


TcpContext::TcpContext(EventLoop* loop)
    :state_(KConnecting),
    index_(-1),
    loop_(loop),
    tdata_(NULL),
    hdata_(new HttpData()),
    channel_(new Channel(loop, -1)){

    channel_->setReadCallback(std::bind(&TcpContext::handleRead, this));
    channel_->setWriteCallback(std::bind(&TcpContext::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpContext::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpContext::handleError, this));
}

TcpContext::~TcpContext(){
    
}

void TcpContext::send(Buffer* buf){
    if(state_ == KConnected){
        if(loop_->isInLoopThread()){
            sendInLoop(buf->peek(), buf->readableBytes());
            buf->retrieveAll();
        }
        else{
            loop_->runInLoop(std::bind(&TcpContext::sendInLoop, this, buf->peek(), buf->readableBytes()));
            buf->retrieveAll();
        }
    }
}

void TcpContext::sendInLoop(const char* data, size_t len){
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    ssize_t remaining = 0;
    bool faultError = false;
    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0){
        nwrote = ::write(channel_->fd(), static_cast<const void*>(data), len);
        if(nwrote >= 0){
            remaining = len - nwrote;
            if(remaining > 0){
                LOG_TRACE << "I am going to write more data.";
            }
        }
        else{
            nwrote = 0;
            if(errno != EWOULDBLOCK){
                if(errno == EPIPE || errno == ECONNRESET || errno == EFAULT || errno == EINTR){
                    faultError = true;
                }
            }
        }
    }
    assert(nwrote >= 0);
    if(!faultError && remaining > 0){
        outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
        if(!channel_->isWriting()){
            channel_->enableWriting();
        }
    }
    else if(!faultError && remaining == 0){
        // loop_->givebackContext(shared_from_this());
        // handleClose();
    }
}

void TcpContext::configEvent(){
    loop_->updateChannel(channel_.get());
    channel_->enableReading();
    hdata_->setHolder(shared_from_this());
}

void TcpContext::handleRead(){
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    if(n > 0){
        requestCallback_(&inputBuffer_);
    }
    else if(n == 0){
        handleClose();
    }
    else{
        errno = savedErrno;
        // LOG_SYSERR << "there is a error"
        handleError();
    }

}

void TcpContext::shutdown(){
    if(state_ == KConnected){
        setState(KDisconnecting);
        loop_->runInLoop(std::bind(&TcpContext::shutdownInLoop, shared_from_this()));
    }
}

void TcpContext::shutdownInLoop(){
    loop_->assertInLoopThread();
    if(!channel_->isWriting()){
        sockets::shutdownWrite(channel_->fd());
    }
}

void TcpContext::handleWrite(){
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
                // LOG_TRACE << "I am going to write more data";
            }
        }
        else{
            // LOG_SYSERR << "failed to write fd";
        }
    }
    else{
        // The Connection is down
    }
}

void TcpContext::handleClose(){
    loop_->assertInLoopThread();
    assert(state_ == KConnected || state_ == KDisconnecting);
    setState(KDisconnected);
    channel_->disableAll();
    ::close(channel_->fd());
    channel_->setFd(-1);
    reset();
    loop_->runInLoop(std::bind(&EventLoop::removeChannel, loop_, channel_.get()));
    loop_->runInLoop(std::bind(&EventLoop::givebackContext, loop_, shared_from_this()));
}

void TcpContext::handleError(){
    int err = sockets::getSocketError(channel_->fd());
    // LOG_ERROR << "there is an error, check the errno";
}

void TcpContext::setChannel(int connfd){
    channel_->setFd(connfd);
    channel_->setHolder(shared_from_this());
}
