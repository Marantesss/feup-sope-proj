CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -lpthread

all: user server #log

user: user.c
	$(CC) $(CFLAGS) -o user.o user.c

server: server.c
	$(CC) $(CFLAGS) -o server.o server.c

#log: log
#	$(CC) $(CFLAGS) -o log.o log.c

clean:
	rm -f *.o *.d