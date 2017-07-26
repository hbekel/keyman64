/* -*- mode: kasm -*- */

:BasicUpstart2(main)
        
.import source "serial.h"

main:   
        jsr serial.open

        lda #$61
        jsr type

        lda #$93
        jsr type
        
        jsr serial.close
        rts

type:   pha

        lda #Command.type
        jsr serial.write

        pla
        jsr serial.write
        rts
        
.import source "serial.asm"
        
	