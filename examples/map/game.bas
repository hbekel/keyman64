10 rem just a "game"
15 poke646,5
20 print"{clr}press a, b, c or d":print
25 print"use the additional buttons connected to the keyman control port b bits 0-3"
27 print
30 geta$
40 if a$ = "a" then print "you pressed a!"
50 if a$ = "b" then print "you pressed b!"
60 if a$ = "c" then print "you pressed c!"
70 if a$ = "d" then print "you pressed d!"
80 goto 30