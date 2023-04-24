include Makefile.inc

CC?=gcc
CFLAGS=-std=gnu99 -Wall -Wno-unused -Wno-expansion-to-defined -O2 -DVERSION=\"$(VERSION)\"
MINGW32?=i686-w64-mingw32
KASM?=java -jar /usr/share/kickassembler/KickAss.jar

PREFIX?=/usr/local
SYSCONFDIR?=/etc
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

.PHONY: all intelhex clean distclean

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
linux: keyman64
win32: keyman64.exe

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
	./intelhex/ihex2bin -i keyman64-application-and-bootloader-$(VERSION).hex -o tmp.bin
	dd if=/dev/zero bs=1024 count=128 | tr "\000" "\377" > $@
	dd if=tmp.bin of=$@ conv=notrunc
	rm -f tmp.bin

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
	rm -rf release

distclean: clean
	rm -rf libusb-1.0*

install: keyman64
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m755 keyman64 $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)$(SYSCONFDIR)/bash_completion.d/
	install -m644 completion/keyman64 \
			$(DESTDIR)$(SYSCONFDIR)/bash_completion.d/

	([ "$(UDEV)x" = "1x" ] && make udev-install) || true

udev-install:
	install -d $(DESTDIR)/etc/udev/rules.d
	install -m644 etc/udev/rules.d/10-keyman64.rules $(DESTDIR)/etc/udev/rules.d

uninstall:
	rm -f $(PREFIX)/bin/keyman64
	([ "$(UDEV)x" = "1x" ] && make udev-uninstall) || true

udev-uninstall:
	rm -f /etc/udev/rules.d/10-keyman64.rules

msi: win32 libusb-1.0.dll
	wixl --arch x86 --define VERSION=$(VERSION) -o ../Keyman64-$(VERSION).msi keyman64.wxs && \
	$(MD5SUM) ../Keyman64-$(VERSION).msi > \
		../Keyman64-$(VERSION).msi.md5

libusb-1.0.26-binaries.7z:
	wget https://github.com/libusb/libusb/releases/download/v1.0.26/libusb-1.0.26-binaries.7z

libusb-1.0.26-binaries: libusb-1.0.26-binaries.7z
	7z x -y libusb-1.0.26-binaries.7z

libusb-1.0.dll: libusb-1.0.26-binaries
	cp libusb-1.0.26-binaries/VS2015-Win32/dll/libusb-1.0.dll .

release: clean keyman64 bin hex win32 libusb-1.0.dll
	mkdir release
	cp *.hex release/
	cp *.bin release/
	cp Changelog release/Changelog.txt
	make -C hardware/gerber
	cp hardware/gerber/*.zip release/
	git archive --prefix=keyman64-$(VERSION)/ -o release/keyman64-$(VERSION).tar.gz HEAD
	wixl --arch x86 --define VERSION=$(VERSION) -o release/keyman64-$(VERSION).msi keyman64.wxs
	for f in release/*.hex; do $(MD5SUM) "$$f" > "$$f".md5; done
	for f in release/*.bin; do $(MD5SUM) "$$f" > "$$f".md5; done
	for f in release/*.gz; do $(MD5SUM) "$$f" > "$$f".md5; done
	for f in release/*.zip; do $(MD5SUM) "$$f" > "$$f".md5; done
	for f in release/*.msi; do $(MD5SUM) "$$f" > "$$f".md5; done
	tar -v -c -z --transform 's/release/keyman64/' -f keyman64.tar.gz release/
	make distclean
