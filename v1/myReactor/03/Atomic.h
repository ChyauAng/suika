#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include "notCopyable.h"
#include <stdint.h>

template<typename T>
class AtomicIntegerT: public notCopyable{
public:
    AtomicIntegerT()
        :value_(0){

    }

    T get(){
        // gcc atomic operation
        return __sync_val_compare_and_swap(&value_, 0, 0);
    }
    
    T getAndAdd(T x){
        return __sync_fetch_and_add(&value_, x);
    }

    T addAndGet(T x){
        return getAndAdd(x) + x;
    }


    T incrementAndGet(){
        return addAndGet(1);
    }

private:
    volatile T value_;
};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;

#endif
