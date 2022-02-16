CC = gcc
CFLAGS = -c -Wall -Iinclude -o linked_list.o -ggdb
SRC = $(wildcard src/*.c)

all: linked_list.o

linked_list.o:
	$(CC) $(CFLAGS) $(SRC)

clean:
	rm -rf linked_list.o
