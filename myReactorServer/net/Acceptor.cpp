#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr,bool reuseport):
    loop_(loop),
    acceptSocket_(sockets::createNonblockingOrDie(listenAddr.family())),
    acceptChannel_(loop, acceptSocket_.fd()),
    listenning_(false),
    idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)){
        assert(idleFd_ >= 0);
        acceptSocket_.setReuseAddr(true);
        acceptSocket_.setReusePort(reuseport);
        acceptSocket_.bindAddress(listenAddr);
        // printf("I am here: Accptor::Acceptor()\n");
        acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor(){
    acceptChannel_.disableAll();
    acceptChannel_.remove();
    ::close(idleFd_);
}

void Acceptor::listen(){
    loop_->assertInLoopThread();
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead(){
    loop_->assertInLoopThread();
    InetAddress peerAddr;
    /* 
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd >= 0){
        if(newConnectionCallback_){
            newConnectionCallback_(connfd, peerAddr);
        }
        else{
            sockets::close(connfd);
        }
    }
    else{
        // LOG_SYSERR
        // no enough file descriptors 
        // close connection gracefully
        if(errno == EMFILE){
            ::close(idleFd_);
            idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
            ::close(idleFd_);
            idleFd_ = ::open("dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
    */
    int connfd = 1;
    while((connfd = acceptSocket_.accept(&peerAddr)) > 0){
        if(newConnectionCallback_){
            newConnectionCallback_(connfd, peerAddr);
        }
        else{
            sockets::close(connfd);
        }
    }

    // LOG_SYSERR
    // if there is no enough file descriptors 
    // close connection gracefully
    if(errno == EMFILE){
        ::close(idleFd_);
        idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
        ::close(idleFd_);
        idleFd_ = ::open("dev/null", O_RDONLY | O_CLOEXEC);
    }
    
    // otherwise no more request comes
}


