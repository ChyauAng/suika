#include "AsyncLogging.h"
#include "LogFile.h"
#include "Timestamp.h"

#include <stdio.h>

AsyncLogging::AsyncLogging(const std::string& basename, int rollSize, int flushInterval)
    :flushInterval_(flushInterval),
    running_(false),
    basename_(basename),
    rollSize_(rollSize),
    thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
    latch_(1),
    mutex_(),
    cond_(mutex_),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_(){
        currentBuffer_->bzeroInner();
        nextBuffer_->bzeroInner();
        buffers_.reserve(16);
    }

void AsyncLogging::append(const char* logline, int len){
    MutexLockGuard lock(mutex_);
    if(currentBuffer_->avail() > len){
        currentBuffer_->append(logline, len);
    }
    else{
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if(nextBuffer_){
            currentBuffer_ = std::move(nextBuffer_);
        }
        else{
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(logline, len);
        cond_.notify();
    }
}

void AsyncLogging::threadFunc(){
    assert(running_ == true);
    latch_.countDown();
    LogFile output(basename_, rollSize_, false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzeroInner();
    newBuffer2->bzeroInner();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(running_){
        assert(newBuffer1 && 0 == newBuffer1->length());
        assert(newBuffer2 && 0 == newBuffer2->length());
        assert(buffersToWrite.empty());

        {
            // for cond_.waitForSeconds(...)
            MutexLockGuard lock(mutex_);
            if(buffers_.empty()){
                cond_.waitForSeconds(flushInterval_);
            }
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();

            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_){
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if(buffersToWrite.size() > 25){
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log message at %s, %zd larger buffers\n", Timestamp::now().toFormattedString().c_str(), buffersToWrite.size() - 2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));

            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for(size_t i = 0; i < buffersToWrite.size(); ++i){
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }

        if(buffersToWrite.size() > 2){
            buffersToWrite.resize(2);
        }

        if(!newBuffer1){
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if(!newBuffer2){
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }
        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}
    



