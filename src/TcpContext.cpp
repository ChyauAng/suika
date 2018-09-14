#include "HttpData.h"
#include "TcpContext.h"


TcpContext::TcpContext(EVentLoop* loop)
    :state_(KConnecting),
    loop_(loop),
    data_(new HttpData()){

}

TcpContext::~TcpContext(){
    
}

void TcpContext::handleRead(){
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);

    if(n > 0){
        //messageCallback(this, &inputBuffer_, t);
        channel_.enableWriting();
    }
    else if(n == 0){
        handleClose();
    }
    else{
        errno = savedErrno;
        // LOG_SYSERR
        handleError();
    }

}

void TcpContext::handleWrite(){
    loop_->assertInLoopThread();
    if(channel_.isWriting()){
        ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
        if(n > 0){
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes() == 0){
                channel_.disableWriting();
                if(state_ == KDisconnecting){
                    sockets::shutdownWrite(channel_->fd());
                }
            }
            else{
            }
        }
        else{
            // LOG_SYSERR
        }
    }
    else{
        // The Connection is down
    }
}

void TcpContext::handleClose(){
    loop_->assertInLoopThread();

    assert(state_ == KConnected || state_ == KDisconnecting);

    setState(KDisConnected);

    channel_->disableAll();

    loop_->removeChannel(channel_);
}

void TcpContext::handleError(){
    int err = sockets::getSocketError(channel_->fd());
    // LOG_ERROR

}

void TcpContext::setChannel(int connfd){
    channel_.reset(Channel(loop_, connfd));
    channel_.setReadCallback(std::bind(&TcpContext::handleRead, this));
    channel_.setWriteCallback(std::bind(&TcpContext::handleWrite, this));
    channel_.setCloseCallback(std::bind(&TcpContext::handleClose, this));
    channel_.setErrorCallback(std::bind(&TcpContext::handleError, this));
    channel_.enableReading();
}
