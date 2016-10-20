all: disk.d64

loader.prg: loader.bas
	petcat -w2 $< > $@

game.prg: game.bas
	petcat -w2 $< > $@

load: loader.prg
	xlink load loader.prg

test: load
	xlink run

disk.d64: loader.prg game.prg
	c1541 -format "loader test,00" d64 disk.d64
	c1541 -attach disk.d64 -write loader.prg loader
	c1541 -attach disk.d64 -write game.prg game

clean:
	rm -f loader.prg
	rm -f game.prg
	rm -f disk.d64
