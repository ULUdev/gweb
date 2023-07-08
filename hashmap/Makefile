CC = gcc
CFLAGS = -c -fPIC -std=c18 -Wall -Iinclude -O3
AR = ar
ARFLAGS = -crs
SRC = $(wildcard src/*.c)

.PHONY: all clean

all: libhashmap.a

%.o: %.c
ifeq ($(COMPACT_OUTPUT),1)
	@echo CC $@
	@$(CC) $^ $(CFLAGS) -o $@
else
	$(CC) $^ $(CFLAGS) -o $@
endif

libhashmap.a: $(SRC:.c=.o)
ifeq ($(COMPACT_OUTPUT),1)
	@echo AR $@
	@$(AR) $(ARFLAGS) $@ $^
else
	$(AR) $(ARFLAGS) $@ $^
endif

clean:
	rm -rf libhashmap.a
