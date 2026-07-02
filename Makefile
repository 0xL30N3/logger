CXX = gcc
LIBRARIES := -levdev -lpthread
INCLUDES := -I/usr/include/libevdev-1.0
CXXFLAGS = -Wall -g $(INCLUDES)

all: loggerd

loggerd: main.c
	$(CXX) $(CXXFLAGS) -o loggerd main.c $(LIBRARIES) 

clean:
	rm -f loggerd *.o
