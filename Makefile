VERSION=1.3

CC?=gcc
CFLAGS=-std=gnu99 -Wall -Wno-unused -O2 -DVERSION=$(VERSION)
MINGW32?=i686-w64-mingw32
KASM?=java -jar /usr/share/kickassembler/KickAss.jar

PREFIX?=/usr/local
DESTDIR=

UNAME=$(shell uname)

MD5SUM=md5sum
ifeq ($(UNAME), Darwin)
  MD5SUM=md5 -r
endif

UDEV=0
ifeq ($(UNAME), Linux)
  UDEV=1
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

print-%: ; @echo $*=$($*)

all: linux
linux: firmware keyman64
win32: firmware keyman64.exe

keyman64: $(HEADERS) $(SOURCES) config.c
	$(CC) $(CFLAGS) -o keyman64 $(SOURCES) $(LIBS)

keyman64.exe: $(HEADERS) $(SOURCES) config.c
	$(MINGW32)-gcc $(CFLAGS) -o keyman64 $(SOURCES) $(LIBS)

firmware: firmware/main.hex

firmware/main.hex: firmware/main.h firmware/main.c firmware/encoding.h keyboard.h config.h config.c 
	(cd firmware && make)

firmware-clean:
	(cd firmware && make clean)
	if [ "x$$OSTYPE" = "xcygwin" ]; then \
	git checkout firmware/usbdrv/usbdrvasm.S; else true; fi

program: firmware
	(cd firmware && make program)

config: keyman64
	./keyman64 example.conf example.bin && \
	avrdude -p m1284p -c usbasp -U eeprom:w:example.bin:r

test: test.conf keyman64
	rm -rf tmp
	mkdir tmp
	./keyman64 convert test.conf tmp/test.bin
	./keyman64 convert tmp/test.bin tmp/roundtrip.conf
	./keyman64 convert tmp/roundtrip.conf tmp/roundtrip.bin
	diff tmp/test.bin tmp/roundtrip.bin
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
	([ "$(UDEV)x" = "1x" ] && make udev-install) || true

udev-install:
	install -d $(DESTDIR)/etc/udev/rules.d
	install -m644 etc/udev/rules.d/10-keyman64.rules $(DESTDIR)/etc/udev/rules.d

uninstall:
	rm -f $(PREFIX)/bin/keyman64
	([ "$(UDEV)x" = "1x" ] && make udev-uninstall) || true

udev-uninstall:
	rm -f /etc/udev/rules.d/10-keyman64.rules

release: clean
	git archive --prefix=keyman64-$(VERSION)/ -o ../keyman64-$(VERSION).tar.gz HEAD && \
	$(MD5SUM) ../keyman64-$(VERSION).tar.gz > ../keyman64-$(VERSION).tar.gz.md5
