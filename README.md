# My Reactor implementation <br>
(Reference: [muduo project](https://github.com/chenshuo/muduo)) <br>

* task00: <br>
    A do-nothing event loop. <br>
    (config.h CurrentThread.cpp CurrentThread.h EventLoop.cpp EventLoop.h main.cpp Makefile notCopyable.h Thread.cpp Thread.h) <br>

* task01: <br>
    Prototype of reactor: Channel, Poller. <br>
    (Channel.cpp Channel.h Poller.cpp Poller.h Timestamp.cpp Timestamp.h) <br>

* task02: <br>
    TimerQueue implementation. <br>
    (Atomic.h Timer.h Timer.cpp TimerId.h TimerQueue.h TimerQueue.cpp) <br>

* task03: <br>
    EventLoop::runInLoop(), EventLoopThread, Mutex, Condition, CountDownLatch <br>
    (Condition.h, Condition.cpp, Mutex.h, CountDownLatch.h CountDownLatch.cpp) <br>

* task04: <br>
    Acceptor Implementation and some encapsulation work. <br>
    (Acceptor.h Acceptor.cpp SocketsOps.h SocketsOps.cpp InetAddress.h InetAddress.cpp Socket.h Socket.cpp Endian.h) <br>

* task05: <br>
    Simple TcpServer and TcpConnection with create and passsive close implementation. <br>
    (TcpServer.h TcpSerevr.cpp TcpConnection.h TcpConnection.cpp) <br>

* task06: <br>
    TcpConnection read and write implementation with buffer. <br>

* task07: <br>
    Multi-threads TcpServer implementation. <br>
    (EventLoopThreadPool.h EventLoopThreadPool.cpp) <br>

* task08: <br>
    TcpClient implementation. <br>
    (TcpClient.h TcpClient.cpp Connector.h Connector.cpp) <br>

* task09: <br>
    Level-triggered EPollPoller implementation. <br>
    (EPollPoller.h EPollPoller.cpp) <br>

* task10: <br>
    Asynchronous multi-threads logging mechanism with four buffers implementation. <br>
    (Logging.h Logging.cpp AsyncLogging.h AsyncLogging.cpp LogFile.h LogFile.cpp LogStream.h LogStream.cpp FileUtil.h FileUtil.cpp) <br>

* task11: <br>
    A little step beyond muduo. <br>
    (Done: Acceptor::handleRead(), Buffer::makeSpace(), EventLoop::loop(), EventLoop::runInLoop(), TcpConnection::sendInLoop(), TcpConnection::shutdown(), Connector::start(), Connector::stop(), Connector::connecting(), Connector::removeAndReset(); <br>
    Todo: <br> 
    the possible double destruct problem in TcpServer::newConnection() && TcpServer::removeConnection(), TcpClient::newConnection() && TcpClient::~TcpClient(); <br>
    add TimerPool.) <br>
