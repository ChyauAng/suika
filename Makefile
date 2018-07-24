TARGET := main

SOURCE := $(wildcard *.c) $(wildcard *.cpp)
OBJS := $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCE)))

CC := gcc
CXX := g++

LIBS :=
LDFLAGS :=

DEFINES :=
INCLUDE :=

CFLAGS := -g -Wall -o3 $(DEFINES) $(INCLUDE)
CXXFLAGS := $(CFLAGS) -std=c++11 -pthread

.PHONY : all clean rebuild

all: $(TARGET)
clean: rm -f *.o
	rm $(TARGET)
rebuild: clean all

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
