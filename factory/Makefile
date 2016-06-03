PROGRAMMER=minipro -d ATMEGA1284P
FUSES=-c config
CODE=-c code
READ=-r
WRITE=-w

all:
	@echo "use 'clone' target to read data from preprogrammed atmega"
	@echo "use 'write' target to write to empty atmega"

clone: clean progmem fuses

write: progmem fuses
	@echo -ne "\ninsert target..."
	@read -n1	
	$(PROGRAMMER) $(PROGRAM) $(WRITE) progmem
	$(PROGRAMMER) $(FUSES) $(WRITE) fuses

progmem:
	@echo -ne "\ninsert master..."
	@read -n1
	$(PROGRAMMER) $(PROGRAM) $(READ) progmem

fuses:
	$(PROGRAMMER) $(FUSES) $(READ) fuses

clean:
	rm -rf progmem fuses

