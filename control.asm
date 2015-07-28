.pc = $c000

main:   {
        lda #$10 
        jsr control
        rts
}        

//------------------------------------------------------------------------------
  
control: {

init:	sei
        sta value

        lda $01  // set port bit 4 und 3 (SENSE & DATA) high
        ora #$18
        sta $01
        
	lda $00   // set port bit 4 und 3 (SENSE & DATA) high
	ora #$18
	sta $00

	ldy #$08  // shift bits out, lsb first...

shift:	lsr value
	bcc clear

set:    ora #$08  // set bit 3 (DATA)
	sta $01
	jmp send
	
clear:
	and #$f7  // clear bit 3 (DATA)
	sta $01

send:	jsr clock  // send clock pulse

next:	dey
	bne shift
	
reset:  lda $00   // reset port bit 4 (SENSE) to input (default)
	and #$ef
	sta $00

done:	cli
	rts

clock: {
	and #$ef  // pull port bit 4 (SENSE) low...
	sta $01

	ora #$10  // ...and high again
	sta $01

	rts
}

value: .byte $00
}

//------------------------------------------------------------------------------        