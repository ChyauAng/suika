#include "TcpConnection.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "Callbacks.h"
#include "Buffer.h"

#include <stdio.h>
#include <unistd.h>
#include <string>

std::string message;

void onConnection(const TcpConnectionPtr& conn){
    if(conn->connected()){
        printf("onConnection(): new connection [%s] from %s\n", conn->name().c_str(), conn->peerAddress().toIpPort().c_str());
        conn->send(message);
        conn->shutdown();
    }
    else{
        printf("on connection(): conenction [%s] is down\n", conn->name().c_str());
    }
}

void onWriteComplete(const TcpConnectionPtr& conn){
    conn->send(message);
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, ssize_t n){
    printf("onMessage(): recived from connection [%s] with %zd byte data\n", conn->name().c_str(), n);
    buf->retrieveAll();
}

int main(int argc, char** argv){
    printf("main(): pid = %d\n", getpid());

    std::string line;
    for(int i = 33; i < 127; i++){
        line.push_back(char(i));
    }
    line += line;

    for(size_t i = 0; i < 127 -33; i++){
        message += line.substr(i, 72) + '\n';
    }

    InetAddress listenAddr(9988);
    EventLoop loop;

    TcpServer server(&loop, listenAddr, std::string("No.1"));
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.setWriteCompleteCallback(onWriteComplete);
    if(argc > 1){
        server.setThreadNum(atoi(argv[1]));
    }

    server.start();

    // loop.quit();
    
    loop.loop();
}
