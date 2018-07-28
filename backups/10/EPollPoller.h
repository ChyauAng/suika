#ifndef _EPOLLPOLLER_H_
#define _EPOLLPOLLER_H_

#include "Timestamp.h"
#include "EventLoop.h"
#include "notCopyable.h"

#include <map>
#include <vector>

class Channel;

class EPollPoller: public notCopyable{
public:
    typedef std::vector<Channel*> ChannelList;

    EPollPoller(EventLoop* loop);
    ~EPollPoller();

    Timestamp poll(int timeoutMs, ChannelList* activeChannels);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    
    bool hasChannel(Channel* channel) const;

    void assertInLoopThread() const{
        ownerLoop_->assertInLoopThread();
    }

private:
    static const int kInitEventListSize = 16;
    static const char* operationToString(int op);
    
    EventLoop* ownerLoop_;

    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channels_;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);

    typedef std::vector<struct epoll_event> EventList;

    int epollfd_;
    EventList events_;
};

#endif
