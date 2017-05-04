include ../Makefile.inc

PROGRAMMER=minipro -p ATMEGA1284P
FUSES=-c config
PROGRAM=-c code
READ=-r
WRITE=-w

all:
	@echo "use 'write' target to write to empty atmega"

write: progmem fuses
	@echo -ne "\ninsert target..."
	@read -n1
	$(PROGRAMMER) $(FUSES) $(WRITE) fuses
	$(PROGRAMMER) $(PROGRAM) $(WRITE) progmem

progmem: 
	make -C .. bin
	cp ../keyman64-application-and-bootloader-$(VERSION).bin progmem

clean:
	rm -rf progmem

