#include <assert.h>
#include <poll.h>
#include "Channel.h"
#include "Poller.h"

Poller::Poller(EventLoop* loop)
    :ownerLoop_(loop){
        
}

Poller::~Poller(){

}

Timestamp Poller::polll(int timeoutMs, ChannelList* activeChannels){
    int numEvents = poll(pollfds_.data(), pollfds_.size(), timeoutMs);
    Timestamp now(Timestamp::now());

    if(numEvents > 0){
        // LOG_TRACE
        fillActiveChannels(numEvents, activeChannels);
    }
    else if(numEvents == 0){
        // LOG_TRACE
    }
    else{
        // LOG_SYSERR
    }

    return now;
}

void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const{
    for(PollFdList::const_iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && numEvents > 0; ++pfd){
        if(pfd->revents > 0){
            --numEvents;
            ChannelMap::const_iterator ch = channels_.find(pfd->fd);
            assert(ch != channels_.end());
            Channel* channel = ch->second;
            assert(channel->fd() == pfd->fd);
            channel->set_revents(pfd->revents);
            activeChannels->push_back(channel);
        }
    }
}

void Poller::updateChannel(Channel* channel){
    assertInLoopThread();
    // LOG_TRACE
    if(channel->index() < 0){
        // index_ 初始值为-1
        assert(channels_.find(channel->fd()) == channels_.end());
        
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);

        int idx = static_cast<int>(pollfds_.size()) - 1;
channel->setIndex(idx);
        channels_[pfd.fd] = channel;
    }
    else{
        // update a existing one
        assert(channels_.find(channel->fd()) != channels_.end());
        assert(channels_[channel->fd()] == channel);

        int idx = channel->index();
        assert(0 <= (idx) && (idx) < static_cast<int>(pollfds_.size()));
        
        struct pollfd& pfd = pollfds_[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        if(channel->isNoneEvent()){
            // ignore the pollfd
            // pfd.fd = -1;
            pfd.fd = - channel->fd() - 1;
        }
    }
}

void Poller::removeChannel(Channel* channel){
    Poller::assertInLoopThread();
    // LOG_TRACE
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    assert(channel->isNoneEvent());

    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
    const struct pollfd& pfd = pollfds_[idx];
    (void) pfd;
    // muduo book p287
    assert(pfd.fd == -channel->fd() - 1 && pfd.events == channel->events());
    size_t n = channels_.erase(channel->fd());
    assert(1 == n);
    (void) n;
    // channel:poller = n:1, channel:fd = 1:1
    // after removing channel, pollfds_ remove the corresponding fd
    if(static_cast<size_t>(idx) == pollfds_.size() - 1){
        pollfds_.pop_back();
    }
    else{
        int channelAtEnd = pollfds_.back().fd;
        iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
        if(channelAtEnd < 0){
            channelAtEnd = -channelAtEnd - 1;
        }
        channels_[channelAtEnd]->setIndex(idx);
        pollfds_.pop_back();
    }
}

