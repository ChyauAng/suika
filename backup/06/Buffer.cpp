#include "Buffer.h"
#include "SocketsOps.h"

#include <errno.h>
#include <sys/uio.h>

template<typename To, typename From>
inline To implicit_cast(From const& f){
    return f;
}

const size_t Buffer::KCheapPrepend;
const size_t Buffer::KInitialSize;

ssize_t Buffer::readFd(int fd, int* savedErrno){
    // when buffer space is not enough, write in the extrabuf first, then write to the buffer, to save an ioctl()/FIONREAD call cost
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();

    vec[0].iov_base =  begin() + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

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
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }
    
    return n;
}


