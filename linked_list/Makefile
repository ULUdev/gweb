CC = gcc
CFLAGS = -c -fPIC -Wall -Iinclude -O3 -std=c18
ARFLAGS = -crs
SRC = $(wildcard src/*.c)

.PHONY: all clean

all: liblinked_list.a

liblinked_list.a: $(SRC:.c=.o)
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

clean:
	rm -rf liblinked_list.a $(SRC:.c=.o)
