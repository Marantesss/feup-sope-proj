CC=gcc
CFLAGS=-Wall -Werror -pedantic
TARGET=forensic
SRCS=forensic.c

# replaces sources’ extension .c by .o (object file)
OBJS=$(SRCS:.c=.o)

# other variables for libraries and include paths could also be defined
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# forensic.o: forensic.c utils.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o