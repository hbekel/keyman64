CC=gcc
CFLAGS=-std=c99 -Wall -Wno-unused
KASM=kasm

all: firmware interceptor

interceptor: config.h config.c strings.h strings.c range.h range.c symbols.h interceptor.c
	$(CC) $(CFLAGS) -o interceptor strings.c range.c interceptor.c

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

control: control.prg

control.prg: control.asm
	$(KASM) control.asm

test-control: control.prg
	xlink control.prg

clean: firmware-clean	
	rm -rf interceptor
	rm -rf eeprom.bin
	rm -rf control.prg
	rm -rf *.stackdump
