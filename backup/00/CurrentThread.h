#ifndef _CURRENT_THREAD_H
#define _CURRENT_THREAD_H

#include<pthread.h>

namespace CurrentThread{

    extern __thread int t_cachedTid;

    pid_t gettid();
    void cachedTid();

    inline int tid(){
        if(__builtin_expect(t_cachedTid == 0, 0)){
            cachedTid();
        }
        return t_cachedTid;
    }

}

#endif

