/* -*- mode: kasm -*- */
        
serial: {

.var tmp = $02
        
//------------------------------------------------------------------------------        
        
open: {
        lda $01  // set port bits 4 and 3 (SENSE & DATA) high
        ora #$18
        sta $01
        
	lda $00   // set port bits 4 and 3 (SENSE & DATA) to output
	ora #$18
	sta $00
        rts
}       

//------------------------------------------------------------------------------                

close: {
        lda $00   // reset port bit 4 (SENSE) to input (default)
	and #$ef
	sta $00
        rts
}        

//------------------------------------------------------------------------------
        
write: {
        // A: value, X and Y are modifed

        php
        sei

        ldy #$08
        
        ldx tmp
        sta tmp
        lda $01
        
shift:  lsr tmp
	bcc clear

set:    ora #$08  // set bit 3 (DATA)
	sta $01
	jmp strobe
	
clear:  and #$f7  // clear bit 3 (DATA)
	sta $01

strobe:	and #$ef  // pull port bit 4 (SENSE) low...
	sta $01

	ora #$10  // ...and high again
	sta $01

next:	dey
	bne shift
	
done:   stx tmp
        plp
        rts
}

//------------------------------------------------------------------------------        
        
}

