CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic

all: utils queue log user server

utils: utils.c
	$(CC) $(CFLAGS) -c -o utils.o utils.c

queue: queue.c
	$(CC) $(CFLAGS) -c -o queue.o queue.c

log: log.c
	$(CC) $(CFLAGS) -c -o log.o log.c

user: user.c
	$(CC) $(CFLAGS) -o user.o user.c log.o

server: server.c
	$(CC) $(CFLAGS) -pthread -o server.o server.c queue.o utils.o log.o

clean:
	rm -f *.o *.d