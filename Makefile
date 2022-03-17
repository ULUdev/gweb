PREFIX = /usr/local
CC = gcc
CFLAGS = -std=c18 -Wall -Iinclude $(shell pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0) -O3
SRC = $(wildcard src/*.c)
FMT = clang-format
FMTFLAGS = -i
OBJS = lib/linked_list.o lib/hashmap.o

all: gweb

gweb: $(SRC) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

lib/linked_list.o:
	mkdir -p lib
	mkdir -p include/linked_list
	$(MAKE) -C linked_list
	cp linked_list/include/* include/linked_list/
	mv linked_list/linked_list.o lib/linked_list.o

lib/hashmap.o:
	mkdir -p lib include/hashmap
	$(MAKE) -C hashmap
	cp hashmap/include/* include/hashmap/
	mv hashmap/hashmap.o lib/hashmap.o

install: gweb
	mkdir -p $(PREFIX)/share/pixmaps $(PREFIX)/share/applications $(PREFIX)/bin
	mv $< $(PREFIX)/bin/$<
	cp gweb.desktop $(PREFIX)/share/applications/gweb.desktop
	cp gweb.png $(PREFIX)/share/pixmaps/gweb.png

clean:
	rm -rf gweb lib

format: $(SRC)
	$(FMT) $(FMTFLAGS) $^
