CC?=gcc
CFLAGS=-Wall -std=c99 -pedantic -Wextra -Wno-padded -Os

SOURCES=kk_ihex_read.c ihex2bin.c
HEADERS=kk_ihex_read.h

.PHONY: all clean

all: ihex2bin

ihex2bin: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

clean:
	rm -rf ihex2bin

