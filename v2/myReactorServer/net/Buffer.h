#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "StringPiece.h"

#include <algorithm>
#include <vector>
#include <assert.h>
#include <string.h>

class Buffer{
public:
    static const size_t KCheapPrepend = 8;
    static const size_t KInitialSize = 1024;

    explicit Buffer(size_t initialSize = KInitialSize)
        :buffer_(KCheapPrepend + initialSize),
        readerIndex_(KCheapPrepend),
        writerIndex_(KCheapPrepend){
            assert(readableBytes() == 0);
            assert(writableBytes() == initialSize);
            assert(prependableBytes() == KCheapPrepend);
    }

    size_t readableBytes() const{
        return writerIndex_ - readerIndex_;
    }

    size_t writableBytes() const{
        return buffer_.size() - writerIndex_;
    }

    size_t prependableBytes() const{
        return readerIndex_;
    }

    const char* peek() const{
        return begin() + readerIndex_;
    }

    const char* findCRLF() const{
        // const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
        const char* crlf = static_cast<const char*>(memmem(peek(), readableBytes(), kCRLF, 2));
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findCRLF(const char* start) const{
        assert(peek() <= start);
        assert(start <= beginWrite());
        // const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
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
        readerIndex_ = KCheapPrepend;
        writerIndex_ = KCheapPrepend;
    }

    void retrieveUntil(const char* end){
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    string retrieveAllAsString(){
        int length = readableBytes();
        string result(peek(), length);
        retrieve(length);
        return result;
    }
    
    void append(const StringPiece& str){
        append(str.data(), str.size());
    }

    void append(const char* data, size_t len){
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }

    void append(const void* data, size_t len){
        append(static_cast<const char*>(data), len);
    }

    void ensureWritableBytes(size_t len){
        if(writableBytes() < len){
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    char* beginWrite(){
        return begin() + writerIndex_;
    }

    const char* beginWrite() const{
        return begin() + writerIndex_;
    }

    void hasWritten(size_t len){
        assert(len <= writableBytes());
        writerIndex_ += len;
    }

    ssize_t readFd(int fd, int* savedErrno);
    
    StringPiece toStringPiece() const{
        return StringPiece(peek(), static_cast<int>(readableBytes()));
    }

private:
    
    char* begin(){
        return &*buffer_.begin();
        // return buffer_.data();
    }

    const char* begin() const{
        return &*buffer_.begin();
        // return buffer_.data();
    }

    void makeSpace(size_t len){
        /*
        if(writableBytes() + prependableBytes() < len + KCheapPrepend){
            // improvement: use move to avoid initialize cost 
            buffer_.resize(writerIndex_ + len);
        }
        else{
            assert(KCheapPrepend < readerIndex_);
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + KCheapPrepend);
            readerIndex_ = KCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
            assert(readable == readableBytes());
        }
        */

        if(KCheapPrepend < readerIndex_){
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + KCheapPrepend);
            readerIndex_ = KCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
            assert(readable == readableBytes());
        }

        if(writableBytes() < len){
            // try to avoid the init cost of vector::resize()
            buffer_.reserve(writerIndex_ + len);
        }
    }
    
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;

    static const char kCRLF[];
};

#endif
