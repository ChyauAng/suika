#include "Thread.h"
#include "CurrentThread.h"
#include <assert.h>

using namespace std;

struct ThreadData{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    pid_t* tid_;
    CountDownLatch* latch_;

    ThreadData(const ThreadFunc& func,
                pid_t* tid_,
                CountDownLatch* latch)
        :func_(func),
        tid_(tid_),
        latch_(latch){
        
        }

    void runInThread(){
        *tid_ = CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();
        latch_ = NULL;
        
        // forgotten part
        // to-do:try-catch exception handling
        func_();
    }
};


void* startThread(void* obj){
    ThreadData* data =static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}


Thread::Thread(const ThreadFunc& func)
    :started_(false),
    joined_(false),
    tid_(0),
    pthreadId_(0),
    func_(func), 
    latch_(1){
    
    }

Thread::~Thread(){
    if(started_ && !joined_){
        pthread_detach(pthreadId_);
    }
}

void Thread::start(){
    assert(!started_);
    started_ = true;

    ThreadData* data = new ThreadData(func_, &tid_, &latch_);

    if(pthread_create(&pthreadId_, NULL, &startThread, data)){
        started_ = false;
        delete data;
        // printf("I am here in Thread::start()");
        // do some logging things
    }
    else{
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join(){
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}


