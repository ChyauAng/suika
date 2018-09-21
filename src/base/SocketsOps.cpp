#include <assert.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <sys/uio.h>

// #include "Logging.h"
#include "SocketsOps.h"

namespace sockets{

typedef struct sockaddr SA;

// safe upcast: static_cast and const_cast
template<typename To, typename From>
inline To implicit_cast(From const &f){
    return f;
}

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr){
    return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr){
    return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr){
    return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
}

const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr){
    return static_cast<const struct sockaddr_in*>(implicit_cast<const void* >(addr));
}

const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr){
    return static_cast<const struct sockaddr_in6*>(implicit_cast<const void*>(addr));
}

inline uint64_t hostToNetwork64(uint64_t host64){
    return htobe64(host64);
}

inline uint32_t hostToNetwork32(uint32_t host32){
    return htobe32(host32);
}


inline uint16_t hostToNetwork16(uint16_t host16){
    return htobe16(host16);
}

inline uint64_t networkToHost64(uint64_t net64){
    return be64toh(net64);
}


inline uint32_t networkToHost32(uint32_t net32){
    return be32toh(net32);
}


inline uint16_t networkToHost16(uint16_t net16){
    return be16toh(net16);
}

int createNonblockingOrDie(sa_family_t family){
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(sockfd < 0){
        // LOG_SYSFATAL << "createNonblockingOrDie in SockerOps.cpp.";
        // printf("Error in Sockets::createNonblockinOrDie\n");
    }
    return sockfd;
}

int socketBindListen(int port){
    if(port < 0 || port > 65535){
        return -1;
    }

    int listenFd = 0;
    if((listenFd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1){
        return -1;
    }

    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);

    if(::bind(listenFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        return -1;
    }

    if(::listen(listenFd, 2048) == -1){
        return -1;
    }

    if(listenFd == -1){
        close(listenFd);
        return -1;
    }

    return listenFd;
}

int setSocketNonBlocking(int fd){
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1){
        return -1;
    }

    flag |= O_NONBLOCK;

    if(fcntl(fd, F_SETFL, flag) == -1){
        return -1;
    }
    return 0;
}


int connect(int sockfd, const struct sockaddr* addr){
    return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

void bindOrDie(int sockfd, const struct sockaddr* addr){
    int ret = ::bind(sockfd, addr, sizeof(struct sockaddr_in6));
    if(ret < 0){
        // LOG_SYSFATAL << "bindOrDie in SocketsOPs.cpp.";
        // printf("The errno is %d", errno);
        // printf("Error in SocketsOps::bindOrDie\n");
    }
}

void listenOrDie(int sockfd){
    int ret = ::listen(sockfd, SOMAXCONN);
    if(ret < 0){
        // printf("Error in SocketsOps::listenOrDie\n");
        // LOG_SYSFATAL << "listenOrDie in SocketsOPs.cpp";
    }
}

int accept(int sockfd, struct sockaddr_in6* addr){
    socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
    
    int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd < 0){
        int savedErrno = errno;
        // printf("Error in SocketsOps::accept\n");
        // lOG_SYSERR << "accpet in SocketsOps.cpp";
        
        // the meaning of the errno??
        switch (savedErrno){
            // temporary error
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO:
            case EPERM:
            case EMFILE:
                // expected errors
                errno = savedErrno;
                break;
            // fatal error
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                // unexpected errors
                // LOG_FATAL << "unexpected error of accpet "<< savedErrno;
                break;
            default:
                // LOG_FATAL << "unexpected error of accpet "<< savedErrno;
                break;
        }
    }
    return connfd;
}

ssize_t read(int sockfd, void* buf, size_t count){
    return ::read(sockfd, buf, count);
}

// read with multi-buffers
ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt){
    return ::readv(sockfd, iov, iovcnt);
}

ssize_t write(int sockfd, const void* buf, size_t count){
    return ::write(sockfd, buf, count);
}

void close(int sockfd){
    if(::close(sockfd) < 0){
        // printf("Error in SocketsOps::close\n");
        // LOG_SYSERR << "close in SocketsOps.cpp";
    }
}

// half-close in tcp
void shutdownWrite(int sockfd){
    if(::shutdown(sockfd, SHUT_WR) < 0){
        // printf("Error in sockets::shutdownWrite\n");
        // LOG_SYSERR << "shiutdownWrite in SockertsOps.cpp";
    }
}

void toIp(char* buf, size_t size, const struct sockaddr* addr){
    if(addr->sa_family == AF_INET){
        assert(size >= INET_ADDRSTRLEN);
        const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
        ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
    }
    else if(addr->sa_family == AF_INET6){
        assert(size >= INET6_ADDRSTRLEN);
        const struct sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
        ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
    }
}

void toIpPort(char* buf, size_t size, const struct sockaddr* addr){
    toIp(buf, size, addr);
    size_t end = ::strlen(buf);
    const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
    uint16_t port = networkToHost16(addr4->sin_port);
    assert(size > end);
    snprintf(buf + end, size - end, "%u", port);
}

void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr){
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if(::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0){
        // printf("Error in SocketsOps::fromIpPort.\n");
        // LOG_SYSERR << "FromIpPort in SocketsOPs.cpp";
    }
}

void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr){
    addr->sin6_family = AF_INET6;
    addr->sin6_port = hostToNetwork16(port);
    if(::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0){
        // printf("Error in SocketsOps::fromIpPort.\n");
        // LOG_SYSERR << "FromIpPort in SocketsOPs.cpp";
    }
}

int getSocketError(int sockfd){
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);

    // get options at the socket api level on sockets
    if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0){
        return errno;
    }
    else{
        return optval;
    }
}

struct sockaddr_in6 getLocalAddr(int sockfd){
    struct sockaddr_in6 localaddr;
    bzero(&localaddr, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if(::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0){
        // LOG_SYSERR << "getLocalAddr in SocketsOps.cpp";
        // printf("Error in sockets::getLocalAddr\n");
    }
    return localaddr;
}

struct sockaddr_in6 getPeerAddr(int sockfd){
    struct sockaddr_in6 peeraddr;
    bzero(&peeraddr, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if(::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0){
        // LOG_SYSERR << "getPeerAddr in SocketsOPs.cpp";
        // printf("Error in sockets::getPeerAddr\n");
    }
    return peeraddr;
}

bool isSelfConnect(int sockfd){
    struct sockaddr_in6 localaddr = getLocalAddr(sockfd);
    struct sockaddr_in6 peeraddr = getPeerAddr(sockfd);
    if(localaddr.sin6_family == AF_INET){
        const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(&localaddr);
        const struct sockaddr_in* raddr4 = reinterpret_cast<struct sockaddr_in*>(&peeraddr);
        return laddr4->sin_port == raddr4->sin_port && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
    }
    else if(localaddr.sin6_family == AF_INET6){
        return localaddr.sin6_port == peeraddr.sin6_port && memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof localaddr.sin6_addr) == 0;
    }
    else{
        return false;
    }
}

void setTcpNoDelay(int sockfd, bool on){
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
    if(ret < 0 && on){
        // 0 : success; -1 : error, and errno be set.
        // LOG_ERROR << "TCP_NODELAY failed.";
    }
}

void setReuseAddr(int sockfd, bool on){
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
    if(ret < 0 && on){
        // LOG_ERROR << "SO_REUSEADDR failed.";
    }
}

void setReusePort(int sockfd, bool on){
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
    if(ret < 0 && on){
        // LOG_ERROR << "SO_REUSEPORT failed.";
    }
}

void setKeepAlive(int sockfd, bool on){
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
    if(ret < 0 && on){
        // LOG_ERROR << "SO_KEEPALIVE failed.";
    }

}
}
