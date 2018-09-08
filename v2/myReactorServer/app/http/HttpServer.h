#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "../../net/TcpServer.h"

#include <memory>

class HttpRequest;
class HttpResponse;

class HttpServer: public notCopyable{
public:
    typedef std::function<void (const HttpRequest&, HttpResponse*)> HttpCallback;

    HttpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name);
    ~HttpServer();

    EventLoop* getLoop() const{
        return server_.getLoop();
    }

    void setHttpCallback(const HttpCallback& cb){
        httpCallback_ = cb;
    }

    void setThreadNum(int numThreads){
        server_.setThreadNum(numThreads);
    }

    void start();


private:
    TcpServer server_;
    HttpCallback httpCallback_;

    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);
    void onRequest(const TcpConnectionPtr&, const HttpRequest&);

    HttpServer(const HttpServer&);
    HttpServer& operator=(const HttpServer&);

};

#endif
