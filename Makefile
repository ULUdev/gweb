PREFIX = /usr/local
CC = gcc
CFLAGS = -std=c18 -Wall -Iinclude `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0` -ggdb
SRC = $(wildcard src/*.c)
FMT = clang-format
FMTFLAGS = -i

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
	mkdir -p $(PREFIX)/share/pixmaps $(PREFIX)/share/applications
	mv $< $(PREFIX)/bin/$<
	cp gweb.desktop $(PREFIX)/share/applications/gweb.desktop
	cp gweb.png $(PREFIX)/share/pixmaps/gweb.png

clean:
	rm -rf gweb lib $(wildcard src/*.o) gweb.o

format: $(SRC)
	$(FMT) $(FMTFLAGS) $^
