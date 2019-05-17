CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic

all: queue user server

queue: queue.c
	$(CC) $(CFLAGS) -c -o queue.o queue.c

user: user.c
	$(CC) $(CFLAGS) -o user.o user.c

server: server.c
	$(CC) $(CFLAGS) -pthread -o server.o server.c queue.o

#log: log
#	$(CC) $(CFLAGS) -o log.o log.c

clean:
	rm -f *.o *.d