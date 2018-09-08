objects = main.o EventLoop.o Poller.o Channel.o Timestamp.o Thread.o CurrentThread.o Timer.o TimerQueue.o 
	
main: $(objects)
	g++ -g -std=c++11 -pthread -o main $(objects) 

main.o: main.cpp EventLoop.h CurrentThread.h Timestamp.h 
	g++ -g -std=c++11 -c main.cpp

EventLoop.o: EventLoop.cpp EventLoop.h notCopyable.h CurrentThread.h Poller.h Channel.h Timestamp.h TimerId.h
	g++ -g -std=c++11 -c EventLoop.cpp

Poller.o: Poller.cpp Poller.h notCopyable.h Timestamp.h EventLoop.h Channel.h 
	g++ -g -std=c++11 -c Poller.cpp

Channel.o: Channel.cpp Channel.h notCopyable.h EventLoop.h
	g++ -g -std=c++11 -c Channel.cpp

Timestamp.o: Timestamp.cpp Timestamp.h
	g++ -g -std=c++11 -c Timestamp.cpp

Thread.o: Thread.cpp Thread.h notCopyable.h CurrentThread.h
	g++ -g -std=c++11 -pthread -c Thread.cpp

CurrentThread.o: CurrentThread.cpp CurrentThread.h
	g++ -g -std=c++11 -c CurrentThread.cpp

Timer.o: Timer.cpp Timer.h notCopyable.h Timestamp.h Atomic.h Callbacks.h
	g++ -g -std=c++11 -c Timer.cpp

TimerQueue.o: TimerQueue.cpp TimerQueue.h notCopyable.h EventLoop.h Channel.h Timestamp.h Callbacks.h Timer.h TimerId.h
	g++ -g -std=c++11 -c TimerQueue.cpp

clean:
	rm main $(objects)

