CC = gcc
CFLAGS = -Wall -Iinclude -ggdb `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0` -o gweb
SRC = $(wildcard src/*.c)

all: gweb

gweb:
	$(CC) $(CFLAGS) $(SRC)

clean:
	rm -rf gweb
