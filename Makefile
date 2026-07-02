CXX = gcc
LIBRARIES := -levdev
INCLUDES := -I/usr/include/libevdev-1.0
CXXFLAGS = -static -Wall -g $(INCLUDES)

all: loggerd

loggerd: main.c
	$(CXX) $(CXXFLAGS) -o loggerd main.c $(LIBRARIES) 

clean:
	rm -f loggerd *.o
