COMPACT_OUTPUT ?= 1
PREFIX = /usr/local
CC = gcc
CFLAGS = -c -std=c18 -Wall -Iinclude -Ihashmap/include -Ilinked_list/include $(shell pkg-config --cflags gtk+-3.0 webkit2gtk-4.0) -O3
LD = gcc
LDFLAGS = -Llib -llinked_list -lhashmap $(shell pkg-config --libs gtk+-3.0 webkit2gtk-4.0)
AR = ar
ARFLAGS = -crs
SRC = $(wildcard src/*.c)
FMT = clang-format
FMTFLAGS = -i
LIBS = lib/liblinked_list.a lib/libhashmap.a

export COMPACT_OUTPUT

.PHONY: all clean format install test

all: lib lib/liblinked_list.a lib/libhashmap.a gweb

gweb: $(SRC:.c=.o)
ifeq ($(COMPACT_OUTPUT),1)
	@echo LD $@
	@$(LD) $^ $(LDFLAGS) -o $@
else
	$(LD) $^ $(LDFLAGS) -o $@
endif

lib/libgweb.a: $(subst src/main.o,,$(SRC:.c=.o))
ifeq ($(COMPACT_OUTPUT),1)
	@echo AR $@
	@$(AR) $(ARFLAGS) $@ $^
else
	$(AR) $(ARFLAGS) $@ $^
endif

%.o: %.c
ifeq ($(COMPACT_OUTPUT),1)
	@echo CC $@
	@$(CC) $^ $(CFLAGS) -o $@
else
	$(CC) $^ $(CFLAGS) -o $@
endif

lib:
	mkdir -p lib

lib/liblinked_list.a:
	$(MAKE) -C linked_list
	mv linked_list/liblinked_list.a lib/liblinked_list.a

lib/libhashmap.a:
	$(MAKE) -C hashmap
	mv hashmap/libhashmap.a lib/libhashmap.a

install: gweb
	mkdir -p $(PREFIX)/share/pixmaps $(PREFIX)/share/applications $(PREFIX)/bin
	mv $< $(PREFIX)/bin/$<
	cp gweb.desktop $(PREFIX)/share/applications/gweb.desktop
	cp gweb.png $(PREFIX)/share/pixmaps/gweb.png

clean:
	rm -rf gweb lib $(SRC:.c=.o)
	$(MAKE) -C hashmap clean
	$(MAKE) -C linked_list clean
	$(MAKE) -C test clean

format: $(SRC) $(wildcard include/*.h)
	$(FMT) $(FMTFLAGS) $^

test: lib lib/libgweb.a lib/liblinked_list.a lib/libhashmap.a
	$(MAKE) -C test
	./test/mock
