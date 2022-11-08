PREFIX = /usr/local
CC = gcc
CFLAGS = -std=c18 -Wall -Iinclude -Ihashmap/include -Ilinked_list/include $(shell pkg-config --cflags gtk+-3.0 webkit2gtk-4.0) -O3
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 webkit2gtk-4.0)
SRC = $(wildcard src/*.c)
FMT = clang-format
FMTFLAGS = -i
OBJS = lib/linked_list.o lib/hashmap.o

all: gweb

gweb: $(SRC) $(OBJS)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

lib/linked_list.o:
	mkdir -p lib
	$(MAKE) -C linked_list
	mv linked_list/linked_list.o lib/linked_list.o

lib/hashmap.o:
	mkdir -p lib
	$(MAKE) -C hashmap
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
