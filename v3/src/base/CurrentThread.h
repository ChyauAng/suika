#ifndef _CURRENT_THREAD_H_
#define _CURRENT_THREAD_H_

#include<pthread.h>

namespace CurrentThread{

    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread int t_cachedPid;
    extern __thread const char* t_threadName;

    pid_t gettid();
    void cachedTid();

    pid_t getpid();
    void cachedPid();

    inline int tid(){
        if(__builtin_expect(t_cachedTid == 0, 0)){
            cachedTid();
        }
        return t_cachedTid;
    }

    inline int pid(){
        if(__builtin_expect(t_cachedPid == 0, 0)){
            cachedPid();
        }
        return t_cachedPid;
    }
    
    inline const char* tidString(){
        return t_tidString;
    }

    inline int tidStringLength(){
        return t_tidStringLength;
    }

    inline const char* name(){
        return t_threadName;
    }


}

#endif

