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

* tesk07: <br>
