#include <errno.h>
#include <sys/uio.h>

#include "Buffer.h"
#include "SocketsOps.h"

template<typename To, typename From>
inline To implicit_cast(From const& f){
    return f;
}

const char Buffer::kCRLF[] = "\r\n";

const size_t Buffer::KInitialSize;

ssize_t Buffer::readFd(int fd, int* savedErrno){
    // when buffer space is not enough, write in the extrabuf first, then write to the buffer, to save an ioctl()/FIONREAD call cost
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();

    vec[0].iov_base =  buf_ + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = 65536;

    const int iovcnt = (writable < sizeof extrabuf)? 2 : 1;
    // multi-buffer read operation
    const ssize_t n = sockets::readv(fd, vec, iovcnt);

    if(n < 0){
        *savedErrno = errno;
    }
    else if(implicit_cast<size_t>(n) <= writable){
        writerIndex_ += n;
    }
    else{
        writerIndex_ = len_;
        append(extrabuf, n - writable);
    }
    
    return n;
}


