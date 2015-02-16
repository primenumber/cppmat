CXX=clang++
SRCS=$(shell ls *.cpp)
OBJS=$(SRCS:.cpp=.o)
HEADS=$(shell ls *.hpp)
CXXFLAGS=-std=c++14 -Wall -Wextra -O3 -march=native -mtune=native
LIBS=

TARGET=cppmat_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< $(INC)

clean:
	rm -f $(TARGET)
	rm -f *.o
