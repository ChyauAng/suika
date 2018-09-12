#include"Thread.h"
#include"CurrentThread.h"
#include<assert.h>

using namespace std;

struct ThreadData{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    pid_t* tid_;

    ThreadData(const ThreadFunc& func,
                pid_t* tid_)
        :func_(func),
        tid_(tid_){
        
        }

    void runInThread(){
        *tid_ = CurrentThread::tid();
        tid_ = NULL;
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
    func_(func){
    
    }

Thread::~Thread(){
    if(started_ && !joined_){
        pthread_detach(pthreadId_);
    }
}

void Thread::start(){
    assert(!started_);
    started_ = true;

    ThreadData* data = new ThreadData(func_, &tid_);

    if(pthread_create(&pthreadId_, NULL, &startThread, data)){
        started_ = false;
        delete data;
        // do some logging things
    }
}

int Thread::join(){
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}


