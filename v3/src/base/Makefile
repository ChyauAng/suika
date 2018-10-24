TARGET := libbase.a

SOURCE := $(wildcard *.c) $(wildcard *.cpp)
OBJS := $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCE)))

CC := gcc
CXX := g++

LIBS :=
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
	ar rcs $@ $^
