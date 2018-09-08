#include "TcpConnection.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "Callbacks.h"
#include "Buffer.h"

#include <stdio.h>
#include <unistd.h>
#include <string>

std::string message1;
std::string message2;

void onConnection(const TcpConnectionPtr& conn){
    if(conn->connected()){
        printf("onConnection(): new connection [%s] from %s\n", conn->name().c_str(), conn->peerAddress().toIpPort().c_str());
        conn->send(message1);
        conn->send(message2);
        conn->shutdown();
    }
    else{
        printf("on connection(): conenction [%s] is down\n", conn->name().c_str());
    }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, ssize_t n){
    printf("onMessage(): recived from connection [%s] with %zd byte data\n", conn->name().c_str(), n);
    buf->retrieveAll();
}

int main(){
    printf("main(): pid = %d\n", getpid());

    int len1 = 100;
    int len2 = 200;

    message1.resize(len1);
    message2.resize(len2);

    std::fill(message1.begin(), message1.end(), 'A');
    std::fill(message2.begin(), message2.end(), 'B');


    InetAddress listenAddr(9988);
    EventLoop loop;

    TcpServer server(&loop, listenAddr, std::string("No.1"));
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
}
