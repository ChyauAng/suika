objects = main.o EventLoop.o Poller.o Channel.o Timestamp.o Thread.o CurrentThread.o Timer.o TimerQueue.o Condition.o EventLoopThread.o CountDownLatch.o SocketsOps.o InetAddress.o Socket.o Acceptor.o TcpConnection.o TcpServer.o 
	
main: $(objects)
	g++ -g -std=c++11 -pthread -o main $(objects) 

main.o: main.cpp EventLoop.h CurrentThread.h EventLoopThread.h
	g++ -g -std=c++11 -c main.cpp

EventLoop.o: EventLoop.cpp EventLoop.h notCopyable.h CurrentThread.h Poller.h Channel.h Timestamp.h TimerId.h Mutex.h
	g++ -g -std=c++11 -c EventLoop.cpp

Poller.o: Poller.cpp Poller.h notCopyable.h Timestamp.h EventLoop.h Channel.h 
	g++ -g -std=c++11 -c Poller.cpp

Channel.o: Channel.cpp Channel.h notCopyable.h EventLoop.h
	g++ -g -std=c++11 -c Channel.cpp

Timestamp.o: Timestamp.cpp Timestamp.h
	g++ -g -std=c++11 -c Timestamp.cpp

Thread.o: Thread.cpp Thread.h notCopyable.h CurrentThread.h CountDownLatch.h
	g++ -g -std=c++11 -pthread -c Thread.cpp

CurrentThread.o: CurrentThread.cpp CurrentThread.h
	g++ -g -std=c++11 -c CurrentThread.cpp

Timer.o: Timer.cpp Timer.h notCopyable.h Timestamp.h Atomic.h Callbacks.h
	g++ -g -std=c++11 -c Timer.cpp

TimerQueue.o: TimerQueue.cpp TimerQueue.h notCopyable.h EventLoop.h Channel.h Timestamp.h Callbacks.h Timer.h TimerId.h
	g++ -g -std=c++11 -c TimerQueue.cpp

Condition.o: Condition.cpp Condition.h Mutex.h notCopyable.h
	g++ -g -std=c++11 -c Condition.cpp

EventLoopThread.o: EventLoopThread.cpp EventLoopThread.h EventLoop.h Condition.h Mutex.h Thread.h notCopyable.h
	g++ -g -std=c++11 -c EventLoopThread.cpp

CountDownLatch.o: CountDownLatch.cpp CountDownLatch.h Condition.h Mutex.h notCopyable.h
	g++ -g -std=c++11 -c CountDownLatch.cpp

SocketsOps.o: SocketsOps.cpp SocketsOps.h Endian.h
	g++ -g -std=c++11 -c SocketsOps.cpp

InetAddress.o: InetAddress.cpp InetAddress.h StringPiece.h Endian.h SocketsOps.h
	g++ -g -std=c++11 -c InetAddress.cpp

Socket.o: Socket.cpp Socket.h notCopyable.h InetAddress.h SocketsOps.h 
	g++ -g -std=c++11 -c Socket.cpp

Acceptor.o: Acceptor.cpp Acceptor.h notCopyable.h Channel.h Socket.h EventLoop.h InetAddress.h SocketsOps.h
	g++ -g -std=c++11 -c Acceptor.cpp

TcpConnection.o: TcpConnection.h Callbacks.h InetAddress.h notCopyable.h EventLoop.h Socket.h Channel.h Timestamp.h
	g++ -g -std=c++11 -c TcpConnection.cpp

TcpServer.o: TcpServer.h Atomic.h TcpConnection.h notCopyable.h Acceptor.h EventLoop.h SocketsOps.h
	g++ -g -std=c++11 -c TcpServer.cpp

clean:
	rm main $(objects)

