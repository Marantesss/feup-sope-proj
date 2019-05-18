CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic

all: utils queue log user server

utils: utils.c
	$(CC) $(CFLAGS) -c -o utils utils.c

queue: queue.c
	$(CC) $(CFLAGS) -c -o queue queue.c

log: log.c
	$(CC) $(CFLAGS) -c -o log log.c

user: user.c
	$(CC) $(CFLAGS) -o user user.c log

server: server.c
	$(CC) $(CFLAGS) -pthread -o server server.c queue utils log

clean:
	rm -f *.o *.d utils queue log user server