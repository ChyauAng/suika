TARGET := HttpServer

SOURCE := $(wildcard *.c) $(wildcard *.cpp)
OBJS := $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCE)))

CC := gcc
CXX := g++

LIBS := base/libbase.a  
LDFLAGS :=  

DEFINES :=
INCLUDE := -I base/  

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
