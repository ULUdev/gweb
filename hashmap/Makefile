CC = gcc
CFLAGS = -c -std=c18 -Wall -Iinclude -ggdb
SRC = src/hashmap.c

all: hashmap.o

hashmap.o: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf hashmap.o
