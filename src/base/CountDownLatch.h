#ifndef _COUNTDOWNLATCH_H_
#define _COUNTDOWNLATCH_H_

#include "Condition.h"
#include "Mutex.h"
#include "notCopyable.h"

class CountDownLatch: public notCopyable{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount()const;

private:
    int count_;
    mutable MutexLock mutex_;
    Condition condition_;
};

#endif
