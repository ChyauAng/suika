#ifndef _INETADDRESS_H_
#define _INETADDRESS_H_

#include "StringPiece.h"

#include <netinet/in.h>

const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);

// encapsulate the sockaddr_in struct
class InetAddress{
public:
    // Cosntruct an endpoint with given port number
    // Mostly used in TCPServer listening.
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

    InetAddress(StringArg ip, uint16_t port, bool ipv6 = false);

    explicit InetAddress(const struct sockaddr_in& addr)
        :addr_(addr){
    }

    explicit InetAddress(const struct sockaddr_in6& addr)
        :addr6_(addr){
    }

    sa_family_t family() const{
        return addr_.sin_family;
    }

    string toIp() const;
    string toIpPort() const;
    uint16_t toPort() const;

    // POD type
    const struct sockaddr* getSockAddr() const;

    void setSockAddrInet6(const struct sockaddr_in6& addr6){
        addr6_ = addr6;
    }

    uint32_t ipNetEndian() const;
    uint16_t portNetEndian() const{
        return addr_.sin_port;
    }

    static bool resolve(StringArg hostname, InetAddress* result);

private:
    union{
        struct sockaddr_in addr_;
        struct sockaddr_in6 addr6_;
    };
};

#endif
