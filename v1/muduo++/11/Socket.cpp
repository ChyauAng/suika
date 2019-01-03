#include "Socket.h"
#include "InetAddress.h"
#include "SocketsOps.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <stdio.h>

Socket::~Socket(){
    sockets::close(sockfd_);
}

bool Socket::getTcpInfo(struct tcp_info* tcpi) const{
    socklen_t len = sizeof(*tcpi);
    bzero(tcpi, len);
    return 0 == getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len);
}

bool Socket::getTcpInfoString(char* buf, int len) const{
    struct tcp_info tcpi;
    bool ok = getTcpInfo(&tcpi);
    if(ok){
        // /usr/include/linux/tcp.h
        snprintf(buf, len, "unrecovered=%u "
                "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                "lost=%u retrans=%u rtt=%u rttvar=%u "
                "sshthresh=%u cwnd=&=%u total_retrans=%u", 
                tcpi.tcpi_retransmits,
                tcpi.tcpi_rto, tcpi.tcpi_ato,
                tcpi.tcpi_snd_mss, tcpi.tcpi_rcv_mss,
                tcpi.tcpi_lost, tcpi.tcpi_retrans,
                tcpi.tcpi_rtt, tcpi.tcpi_rttvar,
                tcpi.tcpi_snd_ssthresh,
                tcpi.tcpi_snd_cwnd,
                tcpi.tcpi_total_retrans);
    }
    return ok;
}

void Socket::bindAddress(const InetAddress& addr){
    sockets::bindOrDie(sockfd_, addr.getSockAddr());
}

void Socket::listen(){
    sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr){
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof addr);
    int connfd = sockets::accept(sockfd_, &addr);
    if(connfd >= 0){
        peeraddr->setSockAddrInet6(addr);
    }
    return connfd;
}

void Socket::shutdownWrite(){
    sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on){
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
    if(ret < 0 && on){
        // 0 : success; -1 : error, and errno be set.
        // LOG_ERROR << "TCP_NODELAY failed.";
    }
}

void Socket::setReuseAddr(bool on){
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
    if(ret < 0 && on){
        // LOG_ERROR << "SO_REUSEADDR failed.";
    }
}

void Socket::setReusePort(bool on){
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
    if(ret < 0 && on){
        // LOG_ERROR << "SO_REUSEPORT failed.";
    }
}

void Socket::setKeepAlive(bool on){
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
    if(ret < 0 && on){
        // LOG_ERROR << "SO_KEEPALIVE failed.";
    }

}
