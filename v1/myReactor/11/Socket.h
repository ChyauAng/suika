#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "notCopyable.h"

// http://www.cse.scu.edu/~dclark/am_256_graph_theory/linux_2_6_stack/structtcp__info.html
struct tcp_info;

class InetAddress;

class Socket: public notCopyable{
public:
    explicit Socket(int sockfd)
        :sockfd_(sockfd){
    }

    ~Socket();

    int fd() const{
        return sockfd_;
    }

    bool getTcpInfo(struct tcp_info*) const;
    bool getTcpInfoString(char* buf, int len) const;
    
    // abort operations
    void bindAddress(const InetAddress& localaddr);
    void listen();

    int accept(InetAddress* peeraddr);

    void shutdownWrite();
    
    // Set sockfd operations
    void setTcpNoDelay(bool on); // Nagle's algorithm
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int sockfd_;
};
#endif
