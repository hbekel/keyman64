10 g$="game"
15 print:print"mapping buttons to keys..."
20 :
30 :rem configure mappings...
40 :
41 :rem p = keyman port: 0=a, 1=b
42 :rem b = keyman bit : 0-7
43 :rem k = keycode, see keyman64 -k
44 :
45 :gosub 3000
46 :
50 p=1: b=0: k=17: gosub 1000
60 p=1: b=1: k=35: gosub 1000
50 p=1: b=2: k=34: gosub 1000
60 p=1: b=3: k=18: gosub 1000
65 :
70 :goto 2000
80 :
90 :rem do not edit below this line
99 :
1000 poke 780,k:poke 781,p:poke 782,b:sys 49152:return

2000 poke646,0:poke53280,0:poke53281,0
2010 print"{clr}load"chr$(34)g$chr$(34)",8:":print"{down}{down}{down}{down}run:{home}"
2020 poke631,19:poke632,13:poke633,13:poke198,3:end

3000 fori=0to101:reada:poke49212+i,a:next:return
4000 data165,1,9,24,133,1,165,0,9,24,133,0,96,165,0,41,239
4001 data133,0,96,8,120,160,8,166,2,133,2,165,1,70,2,144
4002 data7,9,8,133,1,76,45,192,41,247,133,1,41,239,133,1
4003 data9,16,133,1,136,208,230,134,2,40,96,142,99,192,140,100
4004 data192,141,101,192,32,0,192,169,5,32,20,192,173,99,192,10
4005 data10,10,13,100,192,32,20,192,173,101,192,32,20,192,32,13
4006 data192,96,0,0,0

