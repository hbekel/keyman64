.pc = $e000

.pc = $e71d        
wedge: {
        jmp report
eof:    
}
        
.pc = $fb40
report: {
open:   lda $01  // set port bits 4 and 3 (SENSE & DATA) high
        ora #$18
        sta $01
        
	lda $00   // set port bits 4 and 3 (SENSE & DATA) to output
	ora #$18
	sta $00

sendCommand:    
        lda #01
        ldy #$04
        jsr send

sendCharacter:  
        lda $d7
        ldy #$08
        jsr send

close:  lda $00   // reset port bit 4 (SENSE) to input (default)
	and #$ef
	sta $00
        
continue:   
        lda #$00
        sta $d0
        jmp $e721
        
send: { // A: value, Y: number of bits to send
init:   pha

shift:  pla lsr pha
	bcc clear

set:    lda $01
        ora #$08  // set bit 3 (DATA)
	sta $01
	jmp strobe
	
clear:  lda $01
        and #$f7  // clear bit 3 (DATA)
	sta $01

strobe:	and #$ef  // pull port bit 4 (SENSE) low...
	sta $01

	ora #$10  // ...and high again
	sta $01

next:	dey
	bne shift
	
done:	pla
	rts
}
eof:    
}

.pc = $10000

//------------------------------------------------------------------------------		
	
.function patch(start, end) {
	
	.var offset = start - $e000
	.var count = end-start

.return "dd if=kernal.bin of=keyman64.rom conv=notrunc bs=1 count=" + toIntString(count) + " skip=" + toIntString(offset) + " seek=" + toIntString(offset)
}
	
//------------------------------------------------------------------------------		

.print patch(wedge, wedge.eof)
.print patch(report, report.eof)
        