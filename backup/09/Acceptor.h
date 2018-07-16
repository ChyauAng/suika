#ifndef _ACCEPTOR_H_
#define _ACEPTOR_H_

#include <functional>

#include "notCopyable.h"
#include "Channel.h"
#include "Socket.h"

class EventLoop;
class InetAddress;

class Acceptor: public notCopyable{
public:
    typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb){
        newConnectionCallback_ = cb;
    }

    bool listenning() const{
        return listenning_;
    }

    void listen();

private:
    void handleRead();

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
    int idleFd_;
};

#endif
