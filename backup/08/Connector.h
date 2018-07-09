#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include "InetAddress.h"
#include "notCopyable.h"

#include <memory>

class Channel;
class EventLoop;

class Connector: public notCopyable, public std::enable_shared_from_this<Connector>{
public:
    typedef std::function<void(int sockfd)> NewConnectionCallback;

    Connector(EventLoop* loop, const InetAddress& servAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb){
        newConnectionCallback_ = cb;
    }
    
    // called by any thread
    void start();
    void stop();

    // only called in loop thread
    // assertInLoopThead() be called
    void restart();

    const InetAddress& serverAddress() const{
        return servAddr_;
    }

private:
    enum States{kDisconnected, kConnecting, kConnected};

    static const int kMaxRetryDelayMs = 30 * 1000;
    static const int kInitRetryDelayMs = 500;

    void setState(States s){
        state_ = s;
    }
    
    // called by start() and restart()
    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(int sockfd);

    void handleWrite();
    void handleClose();
    void handleError();

    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

    EventLoop* loop_;
    InetAddress servAddr_;
    bool connect_;
    States state_;
    std::unique_ptr<Channel> channel_;
    NewConnectionCallback newConnectionCallback_;
    int retryDelayMs_;
};




#endif
