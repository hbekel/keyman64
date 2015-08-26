MINGW32=i686-w64-mingw32
CFLAGS=-std=c99 -Wall -Wno-unused -O2
KASM=kasm
CC?=gcc

all: linux
linux: firmware keyman64
win32: firmware keyman64.exe

keyman64: config.h config.c strings.h strings.c range.h range.c symbols.h encoding.h keyman64.c
	$(CC) $(CFLAGS) -o keyman64 strings.c range.c keyman64.c

keyman64.exe: config.h config.c strings.h strings.c range.h range.c symbols.h encoding.h keyman64.c
	$(MINGW32)-gcc $(CFLAGS) -o keyman64 strings.c range.c keyman64.c

firmware: firmware/main.hex

firmware/main.hex: firmware/main.h firmware/main.c config.h config.c
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
	./keyman64 example.conf tmp/example.bin
	./keyman64 tmp/example.bin tmp/roundtrip.conf
	./keyman64 tmp/roundtrip.conf tmp/roundtrip.bin
	diff tmp/example.bin tmp/roundtrip.bin || (rm -rf tmp && false)
	rm -rf tmp

control: control.prg

control.prg: control.asm
	$(KASM) control.asm

test-control: control.prg
	xlink control.prg

clean: firmware-clean	
	rm -rf keyman64
	rm -rf *.{prg,bin,stackdump}

install: keyman64
	install -D -m 755 keyman64 /usr/bin/keyman64

uninstall:
	rm -f /usr/bin/keyman64
