.import source "serial.h"
.pc = $e000

.pc = $e71d        
wedge: {
        jmp report
eof:    
}
        
.pc = $fb38
report: {

open:   jsr serial.open

writeCommand:   
        lda #Command.input
        ldy #$04
        jsr serial.write

writeCharacter: 
        lda $d7
        ldy #$08
        jsr serial.write

close:  jsr serial.close
        
continue:
        lda #$00
        sta $d0
        jmp $e721        

.import source "serial.asm"        

eof:    
}

.pc = $10000

//------------------------------------------------------------------------------		
	
.function patch(start, end) {
	
	.var offset = start - $e000
	.var count = end-start

.return "dd if=kernal.bin of=keyman64.rom conv=notrunc bs=1 "
        + "count=" + toIntString(count) + " "
        + "skip=" + toIntString(offset) + " "
        + "seek=" + toIntString(offset)
}
	
//------------------------------------------------------------------------------		

.print patch(wedge, wedge.eof)
.print patch(report, report.eof)
        