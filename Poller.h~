#ifndef _POLLER_H_
#define _POLLER_H_

#include "notCopyable.h"
#include "Timestamp.h"
#include "EventLoop.h"
#include <map>
#include <vector>

struct pollfd;

class Channel;

class Poller: public notCopyable{
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop);
    ~Poller();

    Timestamp poll(int timeoutMs, ChannelList* activeChannels);

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread(){
        ownerLoop_->assertInLoopThread();
    }

private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannel) const;

    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int, Channel*> ChannelMap;

    EventLoop* ownerLoop_;
    PollFdList pollfds_;
    ChannelMap channels_;

};

#endif

