CXX = gcc
LIBRARIES := -levdev
INCLUDES := -I/usr/include/libevdev-1.0
CXXFLAGS = -Wall -g $(INCLUDES)

all: loggerd

loggerd: main.c
	$(CXX) $(CXXFLAGS) $(LIBRARIES) -o loggerd main.c

clean:
	rm -f loggerd *.o
