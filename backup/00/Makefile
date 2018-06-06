objects = main.o EventLoop.o Thread.o CurrentThread.o
	
main: $(objects)
	g++ -std=c++11 -pthread -o main $(objects) 

main.o: main.cpp EventLoop.h Thread.h
	g++ -std=c++11 -c main.cpp

EventLoop.o: EventLoop.cpp EventLoop.h notCopyable.h CurrentThread.h
	g++ -std=c++11 -c EventLoop.cpp

Thread.o: Thread.cpp Thread.h notCopyable.h CurrentThread.h
	g++ -std=c++11 -pthread -c Thread.cpp

CurrentThread.o: CurrentThread.cpp CurrentThread.h
	g++ -std=c++11 -c CurrentThread.cpp

clean:
	rm main $(objects)

