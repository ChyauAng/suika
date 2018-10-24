#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count)
    :count_(count),
    mutex_(),
    condition_(mutex_){

}

void CountDownLatch::wait(){
    MutexLockGuard lock(mutex_);
    while(count_ > 0){  // loop condition
        condition_.wait();
    }
}

void CountDownLatch::countDown(){
    MutexLockGuard lock(mutex_);
    count_--;
    if(0 == count_){
        condition_.notifyAll();
    }
}

int CountDownLatch::getCount()const{
    MutexLockGuard lock(mutex_);
    return count_;
}

