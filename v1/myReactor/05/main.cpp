#include "TcpConnection.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "Callbacks.h"

#include <stdio.h>
#include <unistd.h>
#include <string>

void onConnection(const TcpConnectionPtr& conn){
    if(conn->connected()){
        printf("onConnection(): new connection [%s] from %s\n", conn->name().c_str(), conn->peerAddress().toIpPort().c_str());
    }
    else{
        printf("on connection(): conenction [%s] is down\n", conn->name().c_str());
    }
}

void onMessage(const TcpConnectionPtr& conn, const char* data, ssize_t n){
    printf("onMessage(): recived from connection [%s] with %zd byte data\n", conn->name().c_str(), n);
}

int main(){
    printf("main(): pid = %d\n", getpid());

    InetAddress listenAddr(9981);
    EventLoop loop;
    TcpServer server(&loop, listenAddr, std::string("one"));
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
}
