VERSION=1.1

CC?=gcc
CFLAGS=-std=gnu99 -Wall -Wno-unused -O2 -DVERSION=$(VERSION)
MINGW32?=i686-w64-mingw32
KASM?=java -jar /usr/share/kickassembler/KickAss.jar
MD5SUM=md5sum

PREFIX?=/usr/local
DESTDIR=

UNAME=$(shell uname)

ifeq ($(UNAME), Darwin)
  MD5SUM=md5 -r
endif

HEADERS=config.h \
	strings.h \
	range.h \
	symbols.h \
	usb.h \
	protocol.h \
	target.h \
	keyman64.h

SOURCES=strings.c \
	range.c \
	usb.c \
	keyman64.c

LIBS=-lusb-1.0

all: linux
linux: firmware keyman64
win32: firmware keyman64.exe

keyman64: $(HEADERS) $(SOURCES) config.c
	$(CC) $(CFLAGS) -o keyman64 $(SOURCES) $(LIBS)

keyman64.exe: $(HEADERS) $(SOURCES) config.c
	$(MINGW32)-gcc $(CFLAGS) -o keyman64 $(SOURCES) $(LIBS)

firmware: firmware/main.hex

firmware/main.hex: firmware/main.h firmware/main.c firmware/encoding.h config.h config.c 
	(cd firmware && make)

firmware-clean:
	(cd firmware && make clean)

program: firmware
	(cd firmware && make program)

config: keyman64
	./keyman64 example.conf example.bin && \
	avrdude -p m1284p -c usbasp -U eeprom:w:example.bin:r

test: example.conf keyman64
	rm -rf tmp
	mkdir tmp
	./keyman64 convert example.conf tmp/example.bin
	./keyman64 convert tmp/example.bin tmp/roundtrip.conf
	./keyman64 convert tmp/roundtrip.conf tmp/roundtrip.bin
	diff tmp/example.bin tmp/roundtrip.bin
	rm -rf tmp

reverse.prg: reverse.asm
	$(KASM) reverse.asm

test-reverse: reverse.prg
	echo "Press a key on the c64. The cursor should start to blink again."
	xlink reverse.prg

clean: firmware-clean	
	rm -rf keyman64
	rm -rf keyman64.exe
	rm -rf *.{prg,bin,stackdump}

install: keyman64
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m755 keyman64 $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/keyman64

release: clean
	git archive --prefix=keyman64-$(VERSION)/ -o ../keyman64-$(VERSION).tar.gz HEAD && \
	$(MD5SUM) ../keyman64-$(VERSION).tar.gz > ../keyman64-$(VERSION).tar.gz.md5
