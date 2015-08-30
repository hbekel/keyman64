/* Simple Test program for reverse keyboard scan
 * Port B is used for output and Port A for input
 * This tests wether the 7422106 is truly bidirectional
 * Exits after keypress on success, else hangs forever
*/        

.pc = $c000

.var porta = $dc00
.var portb = $dc01        
        
.var ddra = $dc02
.var ddrb = $dc03        
        
main: {
        sei

        jsr setup

        lda #$00
        sta portb

!wait:  lda porta
        cmp #$ff
        beq !wait-

!wait:  lda porta
        cmp #$ff
        bne !wait-
        
        jsr reset
        cli
        rts
}

setup: {
        lda #$00 sta ddra
        lda #$ff sta ddrb        
        rts
}

reset: {
        lda #$ff sta ddra
        lda #$00 sta ddrb
        rts
}