#include <assert.h>
#include <poll.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>

#include "Channel.h"
#include "EPollPoller.h"

// for Channel, default index is -1
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop* loop)
        :epollfd_(epoll_create1(EPOLL_CLOEXEC)),
        ownerLoop_(loop),
        events_(EPollPoller::kInitEventListSize)
{
    if(epollfd_ < 0){
        // LOG_SYSFATAL << EPollPoller::EPollPoller";
    }
}

EPollPoller::~EPollPoller(){
    close(epollfd_);
}

void EPollPoller::poll(int timeoutMs, ChannelList* activeChannels){
    // LOG_TRACE << "fd total count " << channels_size();
    int numEvents = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int savedErrno = errno;
    // Timestamp now(Timestamp::now());
    if(numEvents > 0){
        // LOG_TRACE << numEvents << " events happened";
        fillActiveChannels(numEvents, activeChannels);
        if(static_cast<size_t>(numEvents) == events_.size()){
            events_.resize(events_.size() * 2);
        }
    }
    else if(numEvents == 0){
        // LOG_TRACE << "nothing happened.";
    }
    else{
        // if the error is not caused by some signal interrupting the system call
        if(savedErrno != EINTR){
            errno = savedErrno;
            // LOG_SYSERR << "EPollPoller::poll()";
        }
    }
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const{
    assert(static_cast<size_t>(numEvents) <= events_.size());
    for(int i = 0; i < numEvents; i++){
        Channel* channel= static_cast<Channel*>(events_[i].data.ptr);
        int fd = channel->fd();
        // printf("The found fd in fillActiveChannels is %d\n", fd);
        ChannelMap::const_iterator it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);

        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::updateChannel(Channel* channel){
    assertInLoopThread();
    const int index = channel->index();
    // LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events() << " index = " << index;
    if(index == kNew || index == kDeleted){
        int fd = channel->fd();
        if(index == kNew){
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        }
        else{
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else{
        int fd = channel->fd();
        (void)fd;
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        assert(index == kAdded);
        if(channel->isNoneEvent()){
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
        else{
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel* channel){
    assertInLoopThread();
    int fd = channel->fd();
    // printf("The remove channel fd is %lld\n", fd);

    // LOG_TRACE << "fd = " << fd;
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = channels_.erase(fd);
    (void)n;
    assert(1 == n);

    if(index == kAdded){
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void EPollPoller::update(int operation, Channel* channel){
    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();

    // LOG_TRACE << "epoll_ctl op = " << oprerationToString(operation) << " fd = " << fd << " event = {" << channel->eventToString() << "}";
    if(epoll_ctl(epollfd_, operation, fd, &event) < 0){
        if(operation == EPOLL_CTL_DEL){
            // LOG_SYSERR << "epoll_ctl op =" << operationToString(operation) << " fd = " << fd;
        }
        else{
            // LOG_SYSFATAL << "epoll_ctl op =" << operationToString(operation) << " fd = " << fd;
        }
    }
}

const char* EPollPoller::operationToString(int op){
    switch (op){
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            assert(false && "ERROR op");
            return "Unknown Operation";
    }
}

