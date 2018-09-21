#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "StringPiece.h"

// #include "StringPiece.h"

class Buffer{
public:
    static const size_t KInitialSize = 1024;
    static const size_t KMaxPreAlloc = 65536;

    explicit Buffer(size_t initialSize = KInitialSize)
        :readerIndex_(0),
        writerIndex_(0),
        len_(KInitialSize),
        free_(0){
            assert(readableBytes() == 0);
            assert(writableBytes() == initialSize);
            buf_ = (char*)(malloc(initialSize));
    }

    ~Buffer(){
        if(buf_ !=NULL){
            free((void*)buf_);
            buf_ = NULL;
        }
    }

    size_t readableBytes() const{
        return writerIndex_ - readerIndex_;
    }

    size_t writableBytes() const{
        return len_ - writerIndex_;
    }

    const char* peek() const{
        return buf_ + readerIndex_;
    }

    const char* findCRLF() const{
        const char* crlf = static_cast<const char*>(memmem(peek(), readableBytes(), kCRLF, 2));
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findCRLF(const char* start) const{
        assert(peek() <= start);
        assert(start <= beginWrite());
        const char* crlf = static_cast<const char*>(memmem(peek(), readableBytes(), kCRLF, 2));
        return crlf == beginWrite()? NULL : crlf;
    }

    void retrieve(size_t len){
        assert(len <= readableBytes());
        if(len < readableBytes()){
            readerIndex_ += len;
        }
        else{
            retrieveAll();
        }
    }

    void retrieveAll(){
        readerIndex_ = 0;
        writerIndex_ = 0;
    }

    void retrieveUntil(const char* end){
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }
    
    // sendInLoop(.., Buffer::getBuf(), Buffer::readbleBytes())
    // buf->retrieveReadable()
    void retrieveReadable(){
        int length = readableBytes();
        retrieve(length);
    }
    
    void append(const StringPiece& str){
        append(str.data(), str.size());
    }

    void append(const char* data, size_t len){
        // printf("The append content is %s\n", data);
        ensureWritableBytes(len);
        strncpy(beginWrite(), data, len);
        hasWritten(len);
    }

    /*
    void append(const StringPiece& sp){
        append(sp.data(), sp.size());
    }
    */

    void ensureWritableBytes(size_t len){
        if(writableBytes() < len){
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    char* beginWrite(){
        return buf_ + writerIndex_;
    }

    const char* beginWrite() const{
        return buf_ + writerIndex_;
    }

    void hasWritten(size_t len){
        assert(len <= writableBytes());
        writerIndex_ += len;
    }
    
    const char* getBuf()const{
        return buf_;
    
    }
    char* getBuf(){
        return buf_;
    }

    const size_t getLen()const{
        return len_;
    }

    ssize_t readFd(int fd, int* savedErrno);

private:
    
    void makeSpace(size_t len){
        size_t newlen = writerIndex_ + len;
        len_ = newlen;
        if(newlen < KMaxPreAlloc){
            newlen *= 2;
        }
        else{
            newlen += KMaxPreAlloc;
        }

        buf_ = (char*)(realloc(buf_, newlen));
        free_ = newlen - len_;
    }
    
    size_t readerIndex_;
    size_t writerIndex_;

    size_t len_;
    // preallocation length
    size_t free_;

    char* buf_;

    static const char kCRLF[];
};

#endif
