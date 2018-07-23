#ifndef _ASYNCLOGGING_H_
#define _ASYNCLOGGING_H_

#include "CountDownLatch.h"
#include "Mutex.h"
#include "Thread.h"
#include "LogStream.h"
#include "notCopyable.h"

#include <memory>
#include <vector>

class AsyncLogging: public notCopyable{
public:
    AsyncLogging(const std::string& basename, int rollSize, int flushInterval = 3);
    ~AsyncLogging(){
        if(running_){
            stop();
        }
    }

    void append(const char* logline, int len);

    void start(){
        running_ = true;
        thread_.start();
        latch_.wait();
    }
    
    void stop(){
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private:
    AsyncLogging(const AsyncLogging&);
    void operator=(const AsyncLogging&);

    void threadFunc();

    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;

    const int flushInterval_;
    bool running_;
    std::string basename_;
    off_t rollSize_;
    Thread thread_;
    CountDownLatch latch_;
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
};

#endif

