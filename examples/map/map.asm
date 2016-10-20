// -*- mode: kasm -*-        
        
.pc = $c000

.import source "../serial.asm"
.import source "../serial.h"

        sta key        
        stx port
        sty mask

        jsr serial.open

        lda #Command.map
        jsr serial.write
        
        lda port
        asl asl asl
        ora mask
        jsr serial.write

        lda key
        jsr serial.write
        
        jsr serial.close
        rts

port:  .byte $00
mask:  .byte $00
key:   .byte $00
        
        

        