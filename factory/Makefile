PROGRAMMER=minipro -p ATMEGA1284
FUSES=-c config
PROGRAM=-c code
READ=-r
WRITE=-w

all:
	@echo "use 'clone' target to read data from preprogrammed atmega"
	@echo "use 'write' target to write to empty atmega"

clone: clean fuses progmem

write: progmem fuses
	@echo -ne "\ninsert target..."
	@read -n1
	$(PROGRAMMER) $(FUSES) $(WRITE) fuses
	$(PROGRAMMER) $(PROGRAM) $(WRITE) progmem

progmem:
	$(PROGRAMMER) $(PROGRAM) $(READ) progmem

fuses:
	@echo -ne "\ninsert master..."
	@read -n1
	$(PROGRAMMER) $(FUSES) $(READ) fuses

clean:
	rm -rf progmem fuses

