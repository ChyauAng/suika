#include "Connector.h"
#include "Channel.h"
#include "EventLoop.h"
#include "SocketsOps.h"

#include <assert.h>
#include <errno.h>

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& servAddr)
    :loop_(loop),
    servAddr_(servAddr),
    connect_(false),
    state_(kDisconnected),
    retryDelayMs_(kInitRetryDelayMs){
        // LOG_DEBUG << "ctor[" << this << "]";
}

Connector::~Connector(){
    // LOG_DEBUG << "dtor[" << this << "]";
    assert(!channel_);
}

void Connector::start(){
    connect_ = true;
    //loop_->runInLoop(std::bind(&Connector::startInLoop, this));
    loop_->runInLoop(std::bind(&Connector::startInLoop, shared_from_this()));
}

void Connector::startInLoop(){
    loop_->assertInLoopThread();
    assert(state_ == kDisconnected);
    if(connect_){
        connect();
    }
    else{
        // LOG_DEBUG << "do not connect";
    }
}

void Connector::connect(){
    int sockfd = sockets::createNonblockingOrDie(servAddr_.family());
    int ret = sockets::connect(sockfd, servAddr_.getSockAddr());
    int savedErrno = ((0 == ret)?0 : errno);
    
    switch (savedErrno){
        case 0:
        case EINPROGRESS: // in connection process
        case EINTR: // interrupted function call
        case EISCONN: // has connected
            connecting(sockfd);
            break;

        case EAGAIN: // resource not enough
        case EADDRINUSE: // address already in use 
        case EADDRNOTAVAIL: // address not in available
        case ECONNREFUSED: // connection refused
        case ENETUNREACH: // network unreachable
            retry(sockfd);
            break;

        case EACCES: // permission denied due to authority
        case EPERM: // operation not permitted
        case EAFNOSUPPORT: // protocol falimy not supported
        case EALREADY: // the former connect request has not been formed
        case EBADF: // bad file descriptor
        case EFAULT: // bad address
        case ENOTSOCK: // not socket file descriptor
            // LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
            sockets::close(sockfd);
            break;
        default:
            // LOG_SYSERR << "Unexpected error in Connector::connectInLoop " << savedErrno;
            sockets::close(sockfd);
            break;
    }
}

void Connector::restart(){
    loop_->assertInLoopThread();
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}

void Connector::connecting(int sockfd){
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_, sockfd));
    /*
    channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
    channel_->setErrorCallback(std::bind(&Connector::handleError, this));
    */
    channel_->setWriteCallback(std::bind(&Connector::handleWrite, shared_from_this()));
    channel_->setErrorCallback(std::bind(&Connector::handleError, shared_from_this()));

    channel_->enableWriting();
}

int Connector::removeAndResetChannel(){
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();
    // cannot reset channel here for still inside channel::handleEvent(handleWrite() or handleError())
    // so reset operation added to loop queue to be done
    // loop_->queueInLoop(std::bind(&Connector::resetChannel, this));
    loop_->queueInLoop(std::bind(&Connector::resetChannel, shared_from_this()));

    return sockfd;
}
 
void Connector::resetChannel(){
    channel_.reset();
}

void Connector::handleWrite(){
    // LOG_TRACE << "Connector::handleWrite " << state_;

    if(state_ == kConnecting){
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if(err){
            // LOG_WARN << "Connector::handleWrite - SO_ERROR = " << err << " " << strerror_tl(error);
            retry(sockfd);
        }
        else if(sockets::isSelfConnect(sockfd)){
            // LOG_WARN << "Connector::handleWrite - Self connect";
            retry(sockfd);
        }
        else{
            setState(kConnected);
            if(connect_){
                newConnectionCallback_(sockfd);
            }
            else{
                sockets::close(sockfd);
            }
        }
    }
    else{
        assert(state_ == kDisconnected);
    }
}

void Connector::handleError(){
    // LOG_ERROR << "Connector::handleError state = " << state_;
    if(state_ == kConnecting){
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        // LOG_TRACE << "SO_ERROR = " << err << " " << strerror_tl(err);
        retry(sockfd);
    }
}

void Connector::retry(int sockfd){
    sockets::close(sockfd);
    setState(kDisconnected);
    if(connect_){
        // LOG_INFO << "Connector::retry - Retry connecting to " << servAddr_.toIpPort() << " in " retryDelayMs_ << " milliseconds. ";
        loop_->runAfter(retryDelayMs_ / 1000.0, std::bind(&Connector::startInLoop, shared_from_this()));
        retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
    }
    else{
        // LOG_DEBUG << "do not connect";
    }
}

void Connector::stopInLoop(){
    loop_->assertInLoopThread();
    if(state_ == kConnecting){
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::stop(){
    connect_ = false;
    // loop_->queueInLoop(std::bind(&Connector::stopInLoop, this));
    loop_->queueInLoop(std::bind(&Connector::stopInLoop, shared_from_this()));
}
