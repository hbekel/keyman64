10 g$="game"
15 print:print"mapping buttons to keys..."
20 :
30 :rem configure mappings...
40 :
41 :rem p = keyman port: 0=a, 1=b
42 :rem b = keyman bit : 0-7
43 :rem k = keycode, see keyman64 -k
44 :
45 :gosub 4000
46 :
50 p=0: b=0: k=17: gosub 1000
60 p=0: b=1: k=35: gosub 1000
50 p=0: b=2: k=34: gosub 1000
60 p=0: b=3: k=18: gosub 1000
65 :
70 goto 2000
80 :
90 :rem do not edit below this line
99 :

1000 poke 780,k:poke 781,p:poke 782,b:sys 49152:return

2000 poke646,1:poke53280,0:poke53281,0
2010 print"{clr}load"chr$(34)g$chr$(34)",8:":print"{down}{down}{down}{down}run:{home}"
2020 poke631,19:poke632,13:poke633,13:poke198,3:end

4000 fori=0to101:readv:poke49152+i,v:nexti
4010 data141,41,192,142,39,192,140,40
4020 data192,32,42,192,169,5,32,62
4030 data192,173,39,192,10,10,10,13
4040 data40,192,32,62,192,173,41,192
4050 data32,62,192,32,55,192,96,0
4060 data0,0,165,1,9,24,133,1
4070 data165,0,9,24,133,0,96,165
4080 data0,41,239,133,0,96,8,120
4090 data160,8,166,2,133,2,165,1
4100 data70,2,144,7,9,8,133,1
4110 data76,87,192,41,247,133,1,41
4120 data239,133,1,9,16,133,1,136
4130 data208,230,134,2,40,96
