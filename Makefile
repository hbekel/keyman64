VERSION=1.4

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

.PHONY: all clean intelhex

HEADERS=config.h \
	strings.h \
	range.h \
	symbols.h \
	usb.h \
	protocol.h \
	target.h \
	intelhex.h \
	keyman64.h

SOURCES=strings.c \
	range.c \
	usb.c \
	intelhex.c \
	keyman64.c

LIBS=-lusb-1.0

print-%: ; @echo $*=$($*)

all: linux
linux: hex keyman64
win32: hex keyman64.exe

keyman64: $(HEADERS) $(SOURCES) config.c
	$(CC) $(CFLAGS) -o keyman64 $(SOURCES) $(LIBS)

keyman64.exe: $(HEADERS) $(SOURCES) config.c
	$(MINGW32)-gcc $(CFLAGS) -o keyman64 $(SOURCES) $(LIBS)

hex: keyman64-application-$(VERSION).hex \
	keyman64-bootloader-$(VERSION).hex \
	keyman64-application-and-bootloader-$(VERSION).hex \
	keyman64-bootloader-updater-$(VERSION).hex

bin: keyman64-application-and-bootloader-$(VERSION).bin keyman64-application-$(VERSION).bin

intelhex: intelhex/ihex2bin

intelhex/ihex2bin:
	make -C intelhex

intelhex-clean:
	make -C intelhex clean

keyman64-application-$(VERSION).bin: keyman64-application-$(VERSION).hex intelhex
	./intelhex/ihex2bin -i keyman64-application-$(VERSION).hex -o $@

keyman64-application-and-bootloader-$(VERSION).bin: keyman64-application-and-bootloader-$(VERSION).hex intelhex
	./intelhex/ihex2bin -i keyman64-application-and-bootloader-$(VERSION).hex -o $@

keyman64-application-$(VERSION).hex: firmware
	cp firmware/main.hex $@
	chmod -x $@

keyman64-bootloader-$(VERSION).hex: bootloader
	cp bootloader/firmware/main.hex $@
	chmod -x $@

keyman64-application-and-bootloader-$(VERSION).hex: firmware bootloader
	(head -n-1 firmware/main.hex; cat bootloader/firmware/main.hex) > $@
	chmod -x $@

keyman64-bootloader-updater-$(VERSION).hex: bootloader
	cp bootloader/updater/updater.hex $@
	chmod -x $@

bootloader: bootloader/firmware/main.hex

bootloader/firmware/main.hex:
	make -C bootloader

bootloader-clean:
	make -C bootloader clean

firmware: firmware/main.hex

firmware/main.hex: firmware/main.h firmware/main.c firmware/encoding.h keyboard.h config.h config.c 
	VERSION=$(VERSION) make -C firmware

firmware-clean:
	make -C firmware clean
	if [ "x$$OSTYPE" = "xcygwin" ]; then \
	git checkout firmware/usbdrv/usbdrvasm.S; else true; fi

program: keyman64 firmware 
	(./keyman64 boot && sleep 3) || true
	make -C firmware program

config: keyman64
	./keyman64 convert example.conf example.bin && \
	(./keyman64 boot && sleep 3) || true
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

clean: firmware-clean bootloader-clean intelhex-clean
	rm -rf keyman64
	rm -rf keyman64.exe
	rm -rf *prg *.bin *.hex *.exe.stackdump

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
