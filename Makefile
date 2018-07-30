CC = gcc
CFLAGS = -Wall -Werror -g -lm

all: first

first: first.c

	$(CC) $(CFLAGS) -o first first.c

clean:
	rm -f first

