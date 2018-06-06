#include<pthread.h>
#include"CurrentThread.h"
#include"notCopyable.h"

class EventLoop : public notCopyable{
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void assertInLoopThread(){
        if(!isInLoopThread()){
            // do some logging things
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const{
        return threadId_ == CurrentThread::tid();
    }

private:
    void abortNotInLoopThread();
    
    bool looping_;
    const pid_t threadId_;
};
