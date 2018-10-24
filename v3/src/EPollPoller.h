#ifndef _EPOLLPOLLER_H_
#define _EPOLLPOLLER_H_

#include <map>
#include <vector>

#include "EventLoop.h"
#include "base/notCopyable.h"

class Channel;

class EPollPoller: public notCopyable{
public:
    typedef std::vector<Channel*> ChannelList;

    EPollPoller(EventLoop* loop);
    ~EPollPoller();

    void poll(int timeoutMs, ChannelList* activeChannels);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    
    bool hasChannel(Channel* channel) const;

    void assertInLoopThread() const{
        ownerLoop_->assertInLoopThread();
    }

private:
    typedef std::map<int, Channel*> ChannelMap;
    typedef std::vector<struct epoll_event> EventList;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);

    static const int kInitEventListSize = 16;
    static const char* operationToString(int op);
    
    int epollfd_;

    EventLoop* ownerLoop_;

    EventList events_;

    ChannelMap channels_;
};

#endif
