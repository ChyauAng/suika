objects = main.o EventLoop.o Poller.o Channel.o Timestamp.o Thread.o CurrentThread.o
	
main: $(objects)
	g++ -std=c++11 -pthread -o main $(objects) 

main.o: main.cpp EventLoop.h Thread.h
	g++ -std=c++11 -c main.cpp

EventLoop.o: EventLoop.cpp EventLoop.h notCopyable.h CurrentThread.h Poller.h Channel.h
	g++ -std=c++11 -c EventLoop.cpp

Poller.o: Poller.cpp Poller.h notCopyable.h Timestamp.h EventLoop.h Channel.h
	g++ -std=c++11 -c Poller.cpp

Channel.o: Channel.cpp Channel.h notCopyable.h EventLoop.h
	g++ -std=c++11 -c Channel.cpp

Timestamp.o: Timestamp.cpp Timestamp.h
	g++ -std=c++11 -c Timestamp.cpp

Thread.o: Thread.cpp Thread.h notCopyable.h CurrentThread.h
	g++ -std=c++11 -pthread -c Thread.cpp

CurrentThread.o: CurrentThread.cpp CurrentThread.h
	g++ -std=c++11 -c CurrentThread.cpp

clean:
	rm main $(objects)

