#include"CurrentThread.h"

#include<stdint.h>
#include<pthread.h>
#include<sys/syscall.h>
#include<unistd.h>

namespace CurrentThread{ 
    __thread int t_cachedTid = 0;
    
    pid_t gettid(){
        return static_cast<pid_t>(syscall(SYS_gettid));
    }

    void cachedTid(){
        if(t_cachedTid == 0){
            t_cachedTid = gettid();
        }
    }


}
