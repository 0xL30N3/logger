CC = gcc
LDLIBS := -levdev -lpthread
INCLUDES := -I/usr/include/libevdev-1.0
CFLAGS = -Wall -g $(INCLUDES)

all: loggerd

loggerd: main.c
	$(CC) $(CFLAGS) -o loggerd main.c $(LDLIBS) 

clean:
	rm -f loggerd *.o
