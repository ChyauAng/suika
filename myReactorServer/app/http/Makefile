TARGET := HttpServer_test

SOURCE := $(wildcard *.c) $(wildcard *.cpp)
OBJS := $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCE)))

MYREACTOR_SRC := $(wildcard ../../net/*.cpp)
MYREACTOR_OBJS := $(patsubst %.cpp, %.o, $(MYREACTOR_SRC))

CC := gcc
CXX := g++

LIBS :=  ../../net/libmyreactor.a
LDFLAGS :=  

DEFINES :=
INCLUDE := -I .  

CFLAGS := -g -o3 $(DEFINES) $(INCLUDE)
CXXFLAGS := $(CFLAGS) -std=c++11 -pthread -DNDEBUG

.PHONY : all clean rebuild

all: $(TARGET)
clean: 
	rm -f *.o
	rm $(TARGET)
rebuild: clean all

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
