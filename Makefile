MINGW32=i686-w64-mingw32
CFLAGS=-std=c99 -Wall -Wno-unused -O2
KASM=kasm
TEMP?=C:\Temp

all: linux
linux: firmware interceptor
win32: firmware interceptor.exe

interceptor: config.h config.c strings.h strings.c range.h range.c symbols.h interceptor.c
	gcc $(CFLAGS) -o interceptor strings.c range.c interceptor.c

interceptor.exe: config.h config.c strings.h strings.c range.h range.c symbols.h interceptor.c
	$(MINGW32)-gcc $(CFLAGS) -o interceptor strings.c range.c interceptor.c

firmware: firmware/main.hex

firmware/main.hex: firmware/main.h firmware/main.c config.h config.c
	(cd firmware && make)

firmware-clean:
	(cd firmware && make clean)

program: firmware
	(cd firmware && make program)

config: interceptor
	./interceptor > eeprom.bin && \
	avrdude -p m1284p -c usbasp -U eeprom:w:eeprom.bin:r

test: example.conf interceptor
	./interceptor example.conf $(TEMP)/example.bin
	./interceptor $(TEMP)/example.bin $(TEMP)/roundtrip.conf
	./interceptor $(TEMP)/roundtrip.conf $(TEMP)/roundtrip.bin
	diff $(TEMP)/example.bin $(TEMP)/roundtrip.bin

control: control.prg

control.prg: control.asm
	$(KASM) control.asm

test-control: control.prg
	xlink control.prg

clean: firmware-clean	
	rm -rf interceptor
	rm -rf *.{prg,bin,stackdump}

