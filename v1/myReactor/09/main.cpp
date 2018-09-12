#include "TcpConnection.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "TcpClient.h"
#include "Callbacks.h"
#include "Buffer.h"

#include <stdio.h>
#include <unistd.h>
#include <string>

std::string message = "Hello\n";

void onConnection(const TcpConnectionPtr& conn){
    if(conn->connected()){
        printf("onConnection(): new connection [%s] from %s\n", conn->name().c_str(), conn->peerAddress().toIpPort().c_str());
        conn->send(message);
    }
    else{
        printf("on connection(): conenction [%s] is down\n", conn->name().c_str());
    }
}


void onMessage(const TcpConnectionPtr& conn, Buffer* buf, ssize_t n){
    printf("onMessage(): recived from connection [%s] with %zd byte data\n", conn->name().c_str(), n);
    buf->retrieveAll();
}

int main(int argc, char** argv){

    InetAddress listenAddr("localhost", 9988);
    EventLoop loop;

    TcpClient client(&loop, listenAddr, std::string("No.1"));
    client.setConnectionCallback(onConnection);
    client.setMessageCallback(onMessage);
    client.enableRetry();
    client.connect();

    loop.loop();
}
