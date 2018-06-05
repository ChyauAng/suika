#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>

#include"EventLoop.h"
#include"Thread.h"


void threadFunc(){
    printf("threadFunc(): pid:%d, tid:%d\n", getpid(), CurrentThread::tid());

    EventLoop loop;

    loop.loop();
}

int main(){
    printf("main(): pid:%d, tid:%d\n", getpid(), CurrentThread::tid());

    EventLoop loop;

    Thread thread(threadFunc);
    thread.start();

    loop.loop();
    pthread_exit(NULL);
    return 0;
}


/*
EventLoop* g_loop;

void threadFunc(){
    g_loop->loop();
}

int main(){
    EventLoop loop;
    g_loop = &loop;
    Thread t(threadFunc);
    t.start();
    t.join();

    return 0;
}
*/





