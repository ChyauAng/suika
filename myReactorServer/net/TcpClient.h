#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#include "notCopyable.h"
#include "Mutex.h"
#include "TcpConnection.h"

#include <memory>
#include <string>

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient: public notCopyable{
public:
    TcpClient(EventLoop* loop, const InetAddress& servAddr, const std::string& nameArg);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() const{
        MutexLockGuard lock(mutex_);
        return connection_;
    }

    EventLoop* getLoop() const{
        return loop_;
    }

    bool retry() const{
        return retry_;
    }

    void enableRetry(){
        retry_ = true;
    }
    
    const string& name() const{
        return name_;
    }
    
    // not thread safe
    void setConnectionCallback(ConnectionCallback&& cb){
        connectionCallback_ = std::move(cb);
    }

    void setMessageCallback(MessageCallback&& cb){
        messageCallback_ = std::move(cb);
    }

    void setWriteCompleteCallback(WriteCompleteCallback&& cb){
        writeCompleteCallback_ = std::move(cb);
    }

private:
    // not thread safe but in loop
    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* loop_;
    ConnectorPtr connector_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    const string name_;
    bool retry_;
    bool connect_;

    int nextConnId_;
    mutable MutexLock mutex_;
    TcpConnectionPtr connection_;
};

#endif
