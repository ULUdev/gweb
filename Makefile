PREFIX = /usr/local
CC = gcc
CFLAGS = -Wall -Iinclude `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0` -ggdb
SRC = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)

all: gweb

gweb: $(SRC) lib/linked_list.o
	$(CC) $(CFLAGS) -o $@ $^

lib/linked_list.o:
	mkdir -p lib
	mkdir -p include/linked_list
	$(MAKE) -C linked_list
	cp linked_list/include/* include/linked_list/
	mv linked_list/linked_list.o lib/linked_list.o

install: gweb
	mv $< $(PREFIX)/bin/$<

clean:
	rm -rf gweb lib $(wildcard src/*.o) gweb.o
