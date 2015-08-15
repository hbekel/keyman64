EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:cd74hc22106
LIBS:keyman64-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Programmable C64 Keyboard Interceptor and Controller"
Date "15 aug 2015"
Rev "1"
Comp "Henning Bekel"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_20 P3
U 1 1 558199A3
P 1250 5750
F 0 "P3" V 1200 5750 60  0000 C CNN
F 1 "KEYBOARD" V 1300 5750 60  0000 C CNN
F 2 "" H 1250 5750 60  0000 C CNN
F 3 "" H 1250 5750 60  0000 C CNN
	1    1250 5750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 558199D0
P 900 4800
F 0 "#PWR01" H 900 4800 30  0001 C CNN
F 1 "GND" H 900 4730 30  0001 C CNN
F 2 "" H 900 4800 60  0000 C CNN
F 3 "" H 900 4800 60  0000 C CNN
	1    900  4800
	0    1    1    0   
$EndComp
NoConn ~ 900  4900
Text Label 900  5000 2    60   ~ 0
~RESTORE
$Comp
L CONN_20 P2
U 1 1 558432CB
P 1250 3600
F 0 "P2" V 1200 3600 60  0000 C CNN
F 1 "CIA1" V 1300 3600 60  0000 C CNN
F 2 "" H 1250 3600 60  0000 C CNN
F 3 "" H 1250 3600 60  0000 C CNN
	1    1250 3600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 558432D1
P 900 2650
F 0 "#PWR02" H 900 2650 30  0001 C CNN
F 1 "GND" H 900 2580 30  0001 C CNN
F 2 "" H 900 2650 60  0000 C CNN
F 3 "" H 900 2650 60  0000 C CNN
	1    900  2650
	0    1    1    0   
$EndComp
NoConn ~ 900  2750
Text Label 900  2850 2    60   ~ 0
~RESTORE
Text Label 900  3050 2    60   ~ 0
B3
Text Label 900  3750 2    60   ~ 0
B0
Text Label 900  3650 2    60   ~ 0
B1
Text Label 900  3550 2    60   ~ 0
B2
Text Label 900  3350 2    60   ~ 0
B4
Text Label 900  3250 2    60   ~ 0
B5
Text Label 900  3150 2    60   ~ 0
B6
Text Label 900  3450 2    60   ~ 0
B7
Text Label 900  3850 2    60   ~ 0
A0
Text Label 900  4450 2    60   ~ 0
A1
Text Label 900  4350 2    60   ~ 0
A2
Text Label 900  4250 2    60   ~ 0
A3
Text Label 900  4150 2    60   ~ 0
A4
Text Label 900  4050 2    60   ~ 0
A5
Text Label 900  3950 2    60   ~ 0
A6
Text Label 900  4550 2    60   ~ 0
A7
Text Label 2400 3150 0    60   ~ 0
VCC
Text Label 4050 1100 0    60   ~ 0
A0
Text Label 4050 1200 0    60   ~ 0
A1
Text Label 4050 1300 0    60   ~ 0
A2
Text Label 4050 1400 0    60   ~ 0
A3
Text Label 4050 1500 0    60   ~ 0
A4
Text Label 4050 1600 0    60   ~ 0
A5
Text Label 4050 1700 0    60   ~ 0
A6
Text Label 4050 1800 0    60   ~ 0
A7
$Comp
L ATMEGA1284P-P IC1
U 1 1 558192B9
P 3300 5600
F 0 "IC1" H 2450 7480 40  0000 L BNN
F 1 "ATMEGA1284P-P" H 3700 3650 40  0000 L BNN
F 2 "DIL40" H 3300 5600 30  0000 C CIN
F 3 "" H 3300 5600 60  0000 C CNN
	1    3300 5600
	1    0    0    -1  
$EndComp
Text Label 4050 2000 0    60   ~ 0
B0
Text Label 4050 2100 0    60   ~ 0
B1
Text Label 4050 2200 0    60   ~ 0
B2
Text Label 4050 2300 0    60   ~ 0
B3
Text Label 4050 2400 0    60   ~ 0
B4
Text Label 4050 2500 0    60   ~ 0
B5
Text Label 4050 2600 0    60   ~ 0
B6
Text Label 4050 2700 0    60   ~ 0
B7
Text Label 4300 6900 0    60   ~ 0
MC
Text Label 4300 7000 0    60   ~ 0
MD
Text Label 4300 7200 0    60   ~ 0
CS
Text Label 4300 7100 0    60   ~ 0
CD
Text Label 2300 5100 2    60   ~ 0
VCC
Text Label 3100 3600 1    60   ~ 0
VCC
Text Label 3300 3600 1    60   ~ 0
VCC
$Comp
L VCC #PWR03
U 1 1 55A76720
P 10300 4950
F 0 "#PWR03" H 10300 5050 30  0001 C CNN
F 1 "VCC" H 10300 5050 30  0000 C CNN
F 2 "" H 10300 4950 60  0000 C CNN
F 3 "" H 10300 4950 60  0000 C CNN
	1    10300 4950
	-1   0    0    1   
$EndComp
$Comp
L PWR_FLAG #FLG04
U 1 1 55A7672F
P 10300 4850
F 0 "#FLG04" H 10300 4945 30  0001 C CNN
F 1 "PWR_FLAG" H 10300 5030 30  0000 C CNN
F 2 "" H 10300 4850 60  0000 C CNN
F 3 "" H 10300 4850 60  0000 C CNN
	1    10300 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	10300 4850 10300 4950
$Comp
L PWR_FLAG #FLG05
U 1 1 55A767C3
P 10600 4850
F 0 "#FLG05" H 10600 4945 30  0001 C CNN
F 1 "PWR_FLAG" H 10600 5030 30  0000 C CNN
F 2 "" H 10600 4850 60  0000 C CNN
F 3 "" H 10600 4850 60  0000 C CNN
	1    10600 4850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 55A767D2
P 10600 5000
F 0 "#PWR06" H 10600 5000 30  0001 C CNN
F 1 "GND" H 10600 4930 30  0001 C CNN
F 2 "" H 10600 5000 60  0000 C CNN
F 3 "" H 10600 5000 60  0000 C CNN
	1    10600 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	10600 4850 10600 5000
$Comp
L GND #PWR07
U 1 1 55A767E1
P 3100 7600
F 0 "#PWR07" H 3100 7600 30  0001 C CNN
F 1 "GND" H 3100 7530 30  0001 C CNN
F 2 "" H 3100 7600 60  0000 C CNN
F 3 "" H 3100 7600 60  0000 C CNN
	1    3100 7600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 55A767E7
P 3300 7600
F 0 "#PWR08" H 3300 7600 30  0001 C CNN
F 1 "GND" H 3300 7530 30  0001 C CNN
F 2 "" H 3300 7600 60  0000 C CNN
F 3 "" H 3300 7600 60  0000 C CNN
	1    3300 7600
	1    0    0    -1  
$EndComp
$Comp
L 4520 U3
U 1 1 55A767EF
P 6000 3600
F 0 "U3" H 6050 4050 60  0000 C CNN
F 1 "4520" H 6200 3250 60  0000 C CNN
F 2 "" H 6000 3600 60  0000 C CNN
F 3 "" H 6000 3600 60  0000 C CNN
	1    6000 3600
	1    0    0    -1  
$EndComp
$Comp
L 4520 U3
U 2 1 55A767FE
P 6000 4650
F 0 "U3" H 6050 5100 60  0000 C CNN
F 1 "4520" H 6200 4300 60  0000 C CNN
F 2 "" H 6000 4650 60  0000 C CNN
F 3 "" H 6000 4650 60  0000 C CNN
	2    6000 4650
	1    0    0    -1  
$EndComp
Text Label 5400 3450 2    60   ~ 0
MC
Text Label 6600 3250 0    60   ~ 0
CS0
Text Label 6600 3350 0    60   ~ 0
CS1
Text Label 6600 3450 0    60   ~ 0
CS2
Text Label 6600 4300 0    60   ~ 0
RS0
Text Label 6600 4400 0    60   ~ 0
RS1
Text Label 6600 4500 0    60   ~ 0
RS2
NoConn ~ 6600 4600
$Comp
L 7400 U1
U 2 1 55A76CFC
P 5700 1200
F 0 "U1" H 5700 1250 60  0000 C CNN
F 1 "7400" H 5700 1100 60  0000 C CNN
F 2 "~" H 5700 1200 60  0000 C CNN
F 3 "~" H 5700 1200 60  0000 C CNN
	2    5700 1200
	1    0    0    -1  
$EndComp
$Comp
L 7400 U1
U 3 1 55A76D17
P 5700 1850
F 0 "U1" H 5700 1900 60  0000 C CNN
F 1 "7400" H 5700 1750 60  0000 C CNN
F 2 "~" H 5700 1850 60  0000 C CNN
F 3 "~" H 5700 1850 60  0000 C CNN
	3    5700 1850
	1    0    0    -1  
$EndComp
$Comp
L 7400 U1
U 4 1 55A76D32
P 7050 1550
F 0 "U1" H 7050 1600 60  0000 C CNN
F 1 "7400" H 7050 1450 60  0000 C CNN
F 2 "~" H 7050 1550 60  0000 C CNN
F 3 "~" H 7050 1550 60  0000 C CNN
	4    7050 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 1450 6300 1450
Wire Wire Line
	6300 1450 6300 1200
Wire Wire Line
	6300 1850 6300 1650
Wire Wire Line
	6300 1650 6450 1650
Wire Wire Line
	5100 1100 5100 1300
Wire Wire Line
	5100 1750 5100 1950
Wire Wire Line
	5100 1200 5000 1200
Connection ~ 5100 1200
Text Label 5000 1200 2    60   ~ 0
MR
Text Label 7650 1550 0    60   ~ 0
CRES
Text Label 5400 3550 2    60   ~ 0
CRES
Text Label 6600 3550 0    60   ~ 0
RCK
Text Label 5400 4500 2    60   ~ 0
RCK
$Comp
L GND #PWR09
U 1 1 55A76DEF
P 5400 3650
F 0 "#PWR09" H 5400 3650 30  0001 C CNN
F 1 "GND" H 5400 3580 30  0001 C CNN
F 2 "" H 5400 3650 60  0000 C CNN
F 3 "" H 5400 3650 60  0000 C CNN
	1    5400 3650
	0    1    1    0   
$EndComp
$Comp
L GND #PWR010
U 1 1 55A76DFE
P 5400 4700
F 0 "#PWR010" H 5400 4700 30  0001 C CNN
F 1 "GND" H 5400 4630 30  0001 C CNN
F 2 "" H 5400 4700 60  0000 C CNN
F 3 "" H 5400 4700 60  0000 C CNN
	1    5400 4700
	0    1    1    0   
$EndComp
Text Label 4950 1850 2    60   ~ 0
RCK
$Comp
L 4051 U4
U 1 1 55A76EFE
P 8200 3250
F 0 "U4" H 8300 3250 60  0000 C CNN
F 1 "4051" H 8300 3050 60  0000 C CNN
F 2 "" H 8200 3250 60  0000 C CNN
F 3 "" H 8200 3250 60  0000 C CNN
	1    8200 3250
	1    0    0    -1  
$EndComp
Text Label 7500 3650 2    60   ~ 0
CS0
Text Label 7500 3750 2    60   ~ 0
CS1
Text Label 7500 3850 2    60   ~ 0
CS2
$Comp
L 4051 U5
U 1 1 55A76F63
P 8200 4800
F 0 "U5" H 8300 4800 60  0000 C CNN
F 1 "4051" H 8300 4600 60  0000 C CNN
F 2 "" H 8200 4800 60  0000 C CNN
F 3 "" H 8200 4800 60  0000 C CNN
	1    8200 4800
	1    0    0    -1  
$EndComp
Text Label 7500 5200 2    60   ~ 0
RS0
Text Label 7500 5300 2    60   ~ 0
RS1
Text Label 7500 5400 2    60   ~ 0
RS2
Text Label 8900 4200 0    60   ~ 0
MD
$Comp
L GND #PWR011
U 1 1 55A77078
P 8900 2650
F 0 "#PWR011" H 8900 2650 30  0001 C CNN
F 1 "GND" H 8900 2580 30  0001 C CNN
F 2 "" H 8900 2650 60  0000 C CNN
F 3 "" H 8900 2650 60  0000 C CNN
	1    8900 2650
	0    -1   -1   0   
$EndComp
Text Label 7500 2650 2    60   ~ 0
C0
Text Label 7500 2750 2    60   ~ 0
C1
Text Label 7500 2850 2    60   ~ 0
C2
Text Label 7500 2950 2    60   ~ 0
C3
Text Label 7500 3050 2    60   ~ 0
C4
Text Label 7500 3150 2    60   ~ 0
C5
Text Label 7500 3250 2    60   ~ 0
C6
Text Label 7500 3350 2    60   ~ 0
C7
Text Label 7500 4200 2    60   ~ 0
R0
Text Label 7500 4300 2    60   ~ 0
R1
Text Label 7500 4400 2    60   ~ 0
R2
Text Label 7500 4500 2    60   ~ 0
R3
Text Label 7500 4600 2    60   ~ 0
R4
Text Label 7500 4700 2    60   ~ 0
R5
Text Label 7500 4800 2    60   ~ 0
R6
Text Label 7500 4900 2    60   ~ 0
R7
$Comp
L CONN_2 P1
U 1 1 55A772B0
P 1250 1350
F 0 "P1" V 1200 1350 40  0000 C CNN
F 1 "TAPEPORT" V 1300 1350 40  0000 C CNN
F 2 "" H 1250 1350 60  0000 C CNN
F 3 "" H 1250 1350 60  0000 C CNN
	1    1250 1350
	1    0    0    -1  
$EndComp
Text Label 900  1450 2    60   ~ 0
CS
Text Label 900  1250 2    60   ~ 0
CD
$Comp
L ICL7660 U2
U 1 1 55A772D4
P 9550 1600
F 0 "U2" H 9750 2000 70  0000 L CNN
F 1 "ICL7660" H 9600 1150 70  0000 L CNN
F 2 "" H 9550 1600 60  0000 C CNN
F 3 "" H 9550 1600 60  0000 C CNN
	1    9550 1600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR012
U 1 1 55A772ED
P 9500 2250
F 0 "#PWR012" H 9500 2250 30  0001 C CNN
F 1 "GND" H 9500 2180 30  0001 C CNN
F 2 "" H 9500 2250 60  0000 C CNN
F 3 "" H 9500 2250 60  0000 C CNN
	1    9500 2250
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR013
U 1 1 55A772FC
P 9500 950
F 0 "#PWR013" H 9500 1050 30  0001 C CNN
F 1 "VCC" H 9500 1050 30  0000 C CNN
F 2 "" H 9500 950 60  0000 C CNN
F 3 "" H 9500 950 60  0000 C CNN
	1    9500 950 
	1    0    0    -1  
$EndComp
$Comp
L CP1 C4
U 1 1 55A7730B
P 8400 1400
F 0 "C4" H 8450 1500 50  0000 L CNN
F 1 "10uF" H 8450 1300 50  0000 L CNN
F 2 "~" H 8400 1400 60  0000 C CNN
F 3 "~" H 8400 1400 60  0000 C CNN
	1    8400 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 1550 8650 1550
Wire Wire Line
	8650 1550 8650 1600
Wire Wire Line
	8650 1600 8400 1600
Wire Wire Line
	8700 1350 8650 1350
Wire Wire Line
	8650 1350 8650 1200
Wire Wire Line
	8650 1200 8400 1200
NoConn ~ 8700 1750
NoConn ~ 10400 1750
$Comp
L CP1 C5
U 1 1 55A77345
P 10650 1650
F 0 "C5" H 10700 1750 50  0000 L CNN
F 1 "10uF" H 10700 1550 50  0000 L CNN
F 2 "~" H 10650 1650 60  0000 C CNN
F 3 "~" H 10650 1650 60  0000 C CNN
	1    10650 1650
	-1   0    0    1   
$EndComp
Wire Wire Line
	10400 1450 10900 1450
Connection ~ 10650 1450
Text Label 10900 1450 0    60   ~ 0
VEE
$Comp
L GND #PWR014
U 1 1 55A77382
P 10650 1850
F 0 "#PWR014" H 10650 1850 30  0001 C CNN
F 1 "GND" H 10650 1780 30  0001 C CNN
F 2 "" H 10650 1850 60  0000 C CNN
F 3 "" H 10650 1850 60  0000 C CNN
	1    10650 1850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 55A774B7
P 7500 5100
F 0 "#PWR015" H 7500 5100 30  0001 C CNN
F 1 "GND" H 7500 5030 30  0001 C CNN
F 2 "" H 7500 5100 60  0000 C CNN
F 3 "" H 7500 5100 60  0000 C CNN
	1    7500 5100
	0    1    1    0   
$EndComp
$Comp
L GND #PWR016
U 1 1 55A774BD
P 7500 3550
F 0 "#PWR016" H 7500 3550 30  0001 C CNN
F 1 "GND" H 7500 3480 30  0001 C CNN
F 2 "" H 7500 3550 60  0000 C CNN
F 3 "" H 7500 3550 60  0000 C CNN
	1    7500 3550
	0    1    1    0   
$EndComp
Wire Wire Line
	4950 1850 5100 1850
Connection ~ 5100 1850
Text Label 900  5200 2    60   ~ 0
R3
Text Label 900  5900 2    60   ~ 0
R0
Text Label 900  5800 2    60   ~ 0
R1
Text Label 900  5700 2    60   ~ 0
R2
Text Label 900  5500 2    60   ~ 0
R4
Text Label 900  5400 2    60   ~ 0
R5
Text Label 900  5300 2    60   ~ 0
R6
Text Label 900  5600 2    60   ~ 0
R7
Text Label 900  6000 2    60   ~ 0
C0
Text Label 900  6600 2    60   ~ 0
C1
Text Label 900  6500 2    60   ~ 0
C2
Text Label 900  6400 2    60   ~ 0
C3
Text Label 900  6300 2    60   ~ 0
C4
Text Label 900  6200 2    60   ~ 0
C5
Text Label 900  6100 2    60   ~ 0
C6
Text Label 900  6700 2    60   ~ 0
C7
$Comp
L CRYSTAL X1
U 1 1 55A77860
P 2050 4500
F 0 "X1" H 2050 4650 60  0000 C CNN
F 1 "16Mhz" H 2050 4350 60  0000 C CNN
F 2 "~" H 2050 4500 60  0000 C CNN
F 3 "~" H 2050 4500 60  0000 C CNN
	1    2050 4500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2300 4800 2300 4700
Wire Wire Line
	1900 4800 2300 4800
Wire Wire Line
	2300 4200 2300 4300
Wire Wire Line
	1900 4200 2300 4200
$Comp
L C C2
U 1 1 55A778BF
P 1900 4000
F 0 "C2" H 1900 4100 40  0000 L CNN
F 1 "18nF" H 1906 3915 40  0000 L CNN
F 2 "~" H 1938 3850 30  0000 C CNN
F 3 "~" H 1900 4000 60  0000 C CNN
	1    1900 4000
	-1   0    0    1   
$EndComp
$Comp
L C C3
U 1 1 55A778D8
P 1900 5000
F 0 "C3" H 1900 5100 40  0000 L CNN
F 1 "18nF" H 1906 4915 40  0000 L CNN
F 2 "~" H 1938 4850 30  0000 C CNN
F 3 "~" H 1900 5000 60  0000 C CNN
	1    1900 5000
	-1   0    0    1   
$EndComp
Connection ~ 2050 4200
Connection ~ 2050 4800
$Comp
L GND #PWR017
U 1 1 55A77919
P 1900 5200
F 0 "#PWR017" H 1900 5200 30  0001 C CNN
F 1 "GND" H 1900 5130 30  0001 C CNN
F 2 "" H 1900 5200 60  0000 C CNN
F 3 "" H 1900 5200 60  0000 C CNN
	1    1900 5200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR018
U 1 1 55A77928
P 1900 3800
F 0 "#PWR018" H 1900 3800 30  0001 C CNN
F 1 "GND" H 1900 3730 30  0001 C CNN
F 2 "" H 1900 3800 60  0000 C CNN
F 3 "" H 1900 3800 60  0000 C CNN
	1    1900 3800
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR019
U 1 1 55A77A74
P 5850 3200
F 0 "#PWR019" H 5850 3300 30  0001 C CNN
F 1 "VCC" H 5850 3300 30  0000 C CNN
F 2 "" H 5850 3200 60  0000 C CNN
F 3 "" H 5850 3200 60  0000 C CNN
	1    5850 3200
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR020
U 1 1 55A77A8E
P 5850 4250
F 0 "#PWR020" H 5850 4350 30  0001 C CNN
F 1 "VCC" H 5850 4350 30  0000 C CNN
F 2 "" H 5850 4250 60  0000 C CNN
F 3 "" H 5850 4250 60  0000 C CNN
	1    5850 4250
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR021
U 1 1 55A77A94
P 8200 2600
F 0 "#PWR021" H 8200 2700 30  0001 C CNN
F 1 "VCC" H 8200 2700 30  0000 C CNN
F 2 "" H 8200 2600 60  0000 C CNN
F 3 "" H 8200 2600 60  0000 C CNN
	1    8200 2600
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR022
U 1 1 55A77A9A
P 8200 4150
F 0 "#PWR022" H 8200 4250 30  0001 C CNN
F 1 "VCC" H 8200 4250 30  0000 C CNN
F 2 "" H 8200 4150 60  0000 C CNN
F 3 "" H 8200 4150 60  0000 C CNN
	1    8200 4150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR023
U 1 1 55A77AA2
P 5850 3900
F 0 "#PWR023" H 5850 3900 30  0001 C CNN
F 1 "GND" H 5850 3830 30  0001 C CNN
F 2 "" H 5850 3900 60  0000 C CNN
F 3 "" H 5850 3900 60  0000 C CNN
	1    5850 3900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR024
U 1 1 55A77AAF
P 5850 4950
F 0 "#PWR024" H 5850 4950 30  0001 C CNN
F 1 "GND" H 5850 4880 30  0001 C CNN
F 2 "" H 5850 4950 60  0000 C CNN
F 3 "" H 5850 4950 60  0000 C CNN
	1    5850 4950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR025
U 1 1 55A77AB5
P 8200 3900
F 0 "#PWR025" H 8200 3900 30  0001 C CNN
F 1 "GND" H 8200 3830 30  0001 C CNN
F 2 "" H 8200 3900 60  0000 C CNN
F 3 "" H 8200 3900 60  0000 C CNN
	1    8200 3900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR026
U 1 1 55A77ABB
P 8200 5450
F 0 "#PWR026" H 8200 5450 30  0001 C CNN
F 1 "GND" H 8200 5380 30  0001 C CNN
F 2 "" H 8200 5450 60  0000 C CNN
F 3 "" H 8200 5450 60  0000 C CNN
	1    8200 5450
	1    0    0    -1  
$EndComp
$Comp
L C C10
U 1 1 55A77F7E
P 10200 5850
F 0 "C10" H 10200 5950 40  0000 L CNN
F 1 "100nF" H 10206 5765 40  0000 L CNN
F 2 "~" H 10238 5700 30  0000 C CNN
F 3 "~" H 10200 5850 60  0000 C CNN
	1    10200 5850
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR027
U 1 1 55A77F84
P 10200 5650
F 0 "#PWR027" H 10200 5750 30  0001 C CNN
F 1 "VCC" H 10200 5750 30  0000 C CNN
F 2 "" H 10200 5650 60  0000 C CNN
F 3 "" H 10200 5650 60  0000 C CNN
	1    10200 5650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR028
U 1 1 55A77F8A
P 10200 6050
F 0 "#PWR028" H 10200 6050 30  0001 C CNN
F 1 "GND" H 10200 5980 30  0001 C CNN
F 2 "" H 10200 6050 60  0000 C CNN
F 3 "" H 10200 6050 60  0000 C CNN
	1    10200 6050
	1    0    0    -1  
$EndComp
$Comp
L C C11
U 1 1 55A77F90
P 10450 5850
F 0 "C11" H 10450 5950 40  0000 L CNN
F 1 "100nF" H 10456 5765 40  0000 L CNN
F 2 "~" H 10488 5700 30  0000 C CNN
F 3 "~" H 10450 5850 60  0000 C CNN
	1    10450 5850
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR029
U 1 1 55A77F96
P 10450 5650
F 0 "#PWR029" H 10450 5750 30  0001 C CNN
F 1 "VCC" H 10450 5750 30  0000 C CNN
F 2 "" H 10450 5650 60  0000 C CNN
F 3 "" H 10450 5650 60  0000 C CNN
	1    10450 5650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR030
U 1 1 55A77F9C
P 10450 6050
F 0 "#PWR030" H 10450 6050 30  0001 C CNN
F 1 "GND" H 10450 5980 30  0001 C CNN
F 2 "" H 10450 6050 60  0000 C CNN
F 3 "" H 10450 6050 60  0000 C CNN
	1    10450 6050
	1    0    0    -1  
$EndComp
$Comp
L C C12
U 1 1 55A77FA2
P 10700 5850
F 0 "C12" H 10700 5950 40  0000 L CNN
F 1 "100nF" H 10706 5765 40  0000 L CNN
F 2 "~" H 10738 5700 30  0000 C CNN
F 3 "~" H 10700 5850 60  0000 C CNN
	1    10700 5850
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR031
U 1 1 55A77FA8
P 10700 5650
F 0 "#PWR031" H 10700 5750 30  0001 C CNN
F 1 "VCC" H 10700 5750 30  0000 C CNN
F 2 "" H 10700 5650 60  0000 C CNN
F 3 "" H 10700 5650 60  0000 C CNN
	1    10700 5650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR032
U 1 1 55A77FAE
P 10700 6050
F 0 "#PWR032" H 10700 6050 30  0001 C CNN
F 1 "GND" H 10700 5980 30  0001 C CNN
F 2 "" H 10700 6050 60  0000 C CNN
F 3 "" H 10700 6050 60  0000 C CNN
	1    10700 6050
	1    0    0    -1  
$EndComp
Text Label 8900 3750 0    60   ~ 0
VEE
$Comp
L GND #PWR033
U 1 1 55B35D41
P 8900 5300
F 0 "#PWR033" H 8900 5300 30  0001 C CNN
F 1 "GND" H 8900 5230 30  0001 C CNN
F 2 "" H 8900 5300 60  0000 C CNN
F 3 "" H 8900 5300 60  0000 C CNN
	1    8900 5300
	0    -1   -1   0   
$EndComp
$Comp
L CD74HC22106 U6
U 1 1 55B3756A
P 3350 1900
F 0 "U6" H 3000 2800 60  0000 C CNN
F 1 "CD74HC22106" H 3350 950 60  0000 C CNN
F 2 "" H 3300 2600 60  0000 C CNN
F 3 "" H 3300 2600 60  0000 C CNN
	1    3350 1900
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR034
U 1 1 55B37587
P 3350 1050
F 0 "#PWR034" H 3350 1150 30  0001 C CNN
F 1 "VCC" H 3350 1150 30  0000 C CNN
F 2 "" H 3350 1050 60  0000 C CNN
F 3 "" H 3350 1050 60  0000 C CNN
	1    3350 1050
	1    0    0    -1  
$EndComp
Text Label 2650 1100 2    60   ~ 0
CPA0
Text Label 2650 1200 2    60   ~ 0
CPA1
Text Label 2650 1300 2    60   ~ 0
CPA2
Text Label 2650 1400 2    60   ~ 0
CPA3
Text Label 2650 1500 2    60   ~ 0
CPA4
Text Label 2650 1600 2    60   ~ 0
CPA5
Text Label 2650 2050 2    60   ~ 0
~CPS
Text Label 2650 2150 2    60   ~ 0
CPD
Text Label 4300 3900 0    60   ~ 0
CPA0
Text Label 4300 4000 0    60   ~ 0
CPA1
Text Label 4300 4100 0    60   ~ 0
CPA2
Text Label 4300 4200 0    60   ~ 0
CPA3
Text Label 4300 4300 0    60   ~ 0
CPA4
Text Label 4300 4400 0    60   ~ 0
CPA5
Text Label 4300 4500 0    60   ~ 0
~CPS
Text Label 4300 4600 0    60   ~ 0
CPD
$Comp
L GND #PWR035
U 1 1 55B37A62
P 7150 6600
F 0 "#PWR035" H 7150 6600 30  0001 C CNN
F 1 "GND" H 7150 6530 30  0001 C CNN
F 2 "" H 7150 6600 60  0000 C CNN
F 3 "" H 7150 6600 60  0000 C CNN
	1    7150 6600
	0    -1   -1   0   
$EndComp
$Comp
L SW_PUSH SW1
U 1 1 55B37A78
P 6850 6250
F 0 "SW1" H 7000 6360 50  0000 C CNN
F 1 "RESET" H 6850 6170 50  0000 C CNN
F 2 "" H 6850 6250 60  0000 C CNN
F 3 "" H 6850 6250 60  0000 C CNN
	1    6850 6250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR036
U 1 1 55B37A96
P 7150 6250
F 0 "#PWR036" H 7150 6250 30  0001 C CNN
F 1 "GND" H 7150 6180 30  0001 C CNN
F 2 "" H 7150 6250 60  0000 C CNN
F 3 "" H 7150 6250 60  0000 C CNN
	1    7150 6250
	0    -1   -1   0   
$EndComp
Text Label 6550 6250 2    60   ~ 0
~RESET
Text Label 2300 3900 2    60   ~ 0
~RESET
Text Label 5700 6700 0    60   ~ 0
D-
Text Label 5700 6800 0    60   ~ 0
D+
Text Label 4300 4800 0    60   ~ 0
UA0
Text Label 4300 4900 0    60   ~ 0
UA1
Text Label 4300 5000 0    60   ~ 0
UA2
Text Label 4300 5100 0    60   ~ 0
UA3
Text Label 4300 5200 0    60   ~ 0
UA4
Text Label 4300 5300 0    60   ~ 0
UA5
Text Label 4300 5400 0    60   ~ 0
UA6
Text Label 4300 5500 0    60   ~ 0
UA7
Text Label 4300 5700 0    60   ~ 0
UB0
Text Label 4300 5800 0    60   ~ 0
UB1
Text Label 4300 5900 0    60   ~ 0
UB2
Text Label 4300 6000 0    60   ~ 0
UB3
Text Label 4300 6100 0    60   ~ 0
UB4
Text Label 4300 6200 0    60   ~ 0
UB5
Text Label 4300 6300 0    60   ~ 0
UB6
Text Label 4300 6400 0    60   ~ 0
UB7
Text Label 850  6950 2    60   ~ 0
UA0
Text Label 850  7050 2    60   ~ 0
UA1
Text Label 850  7150 2    60   ~ 0
UA2
Text Label 850  7250 2    60   ~ 0
UA3
Text Label 850  7350 2    60   ~ 0
UA4
Text Label 850  7450 2    60   ~ 0
UA5
Text Label 850  7550 2    60   ~ 0
UA6
Text Label 850  7650 2    60   ~ 0
UA7
Text Label 1650 6950 0    60   ~ 0
UB0
Text Label 1650 7050 0    60   ~ 0
UB1
Text Label 1650 7150 0    60   ~ 0
UB2
Text Label 1650 7250 0    60   ~ 0
UB3
Text Label 1650 7350 0    60   ~ 0
UB4
Text Label 1650 7450 0    60   ~ 0
UB5
Text Label 1650 7550 0    60   ~ 0
UB6
Text Label 1650 7650 0    60   ~ 0
UB7
$Comp
L R R3
U 1 1 55B38786
P 5450 6700
F 0 "R3" V 5530 6700 40  0000 C CNN
F 1 "68" V 5457 6701 40  0000 C CNN
F 2 "" V 5380 6700 30  0000 C CNN
F 3 "" H 5450 6700 30  0000 C CNN
	1    5450 6700
	0    -1   -1   0   
$EndComp
$Comp
L R R4
U 1 1 55B38798
P 5450 6800
F 0 "R4" V 5530 6800 40  0000 C CNN
F 1 "68" V 5457 6801 40  0000 C CNN
F 2 "" V 5380 6800 30  0000 C CNN
F 3 "" H 5450 6800 30  0000 C CNN
	1    5450 6800
	0    -1   -1   0   
$EndComp
$Comp
L R R2
U 1 1 55B387AA
P 5200 6450
F 0 "R2" V 5280 6450 40  0000 C CNN
F 1 "1k5" V 5207 6451 40  0000 C CNN
F 2 "" V 5130 6450 30  0000 C CNN
F 3 "" H 5200 6450 30  0000 C CNN
	1    5200 6450
	1    0    0    -1  
$EndComp
$Comp
L ZENER D2
U 1 1 55B387CD
P 5200 7000
F 0 "D2" H 5200 7100 50  0000 C CNN
F 1 "3.6V" H 5200 6900 40  0000 C CNN
F 2 "" H 5200 7000 60  0000 C CNN
F 3 "" H 5200 7000 60  0000 C CNN
	1    5200 7000
	0    -1   -1   0   
$EndComp
$Comp
L ZENER D1
U 1 1 55B387E1
P 4900 6900
F 0 "D1" H 4900 7000 50  0000 C CNN
F 1 "3.6V" H 4900 6800 40  0000 C CNN
F 2 "" H 4900 6900 60  0000 C CNN
F 3 "" H 4900 6900 60  0000 C CNN
	1    4900 6900
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR037
U 1 1 55B387FF
P 5200 6200
F 0 "#PWR037" H 5200 6300 30  0001 C CNN
F 1 "VCC" H 5200 6300 30  0000 C CNN
F 2 "" H 5200 6200 60  0000 C CNN
F 3 "" H 5200 6200 60  0000 C CNN
	1    5200 6200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR038
U 1 1 55B38813
P 5200 7200
F 0 "#PWR038" H 5200 7200 30  0001 C CNN
F 1 "GND" H 5200 7130 30  0001 C CNN
F 2 "" H 5200 7200 60  0000 C CNN
F 3 "" H 5200 7200 60  0000 C CNN
	1    5200 7200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR039
U 1 1 55B38825
P 4900 7100
F 0 "#PWR039" H 4900 7100 30  0001 C CNN
F 1 "GND" H 4900 7030 30  0001 C CNN
F 2 "" H 4900 7100 60  0000 C CNN
F 3 "" H 4900 7100 60  0000 C CNN
	1    4900 7100
	1    0    0    -1  
$EndComp
Text Notes 4750 7400 0    60   ~ 0
USB Level Conversion\n
Wire Wire Line
	4300 6700 5200 6700
Connection ~ 4900 6700
Wire Wire Line
	4300 6800 5200 6800
$Comp
L SW_PUSH SW2
U 1 1 55B3E8F4
P 6850 6600
F 0 "SW2" H 7000 6710 50  0000 C CNN
F 1 "BOOTLOADER" H 6850 6520 50  0000 C CNN
F 2 "" H 6850 6600 60  0000 C CNN
F 3 "" H 6850 6600 60  0000 C CNN
	1    6850 6600
	1    0    0    -1  
$EndComp
$Comp
L 7400 U1
U 1 1 55A76CA9
P 10400 6750
F 0 "U1" H 10400 6800 60  0000 C CNN
F 1 "7400" H 10400 6650 60  0000 C CNN
F 2 "~" H 10400 6750 60  0000 C CNN
F 3 "~" H 10400 6750 60  0000 C CNN
	1    10400 6750
	1    0    0    -1  
$EndComp
NoConn ~ 11000 6750
$Comp
L GND #PWR040
U 1 1 55B4B074
P 9800 6650
F 0 "#PWR040" H 9800 6650 30  0001 C CNN
F 1 "GND" H 9800 6580 30  0001 C CNN
F 2 "" H 9800 6650 60  0000 C CNN
F 3 "" H 9800 6650 60  0000 C CNN
	1    9800 6650
	0    1    1    0   
$EndComp
$Comp
L GND #PWR041
U 1 1 55B4B08B
P 9800 6850
F 0 "#PWR041" H 9800 6850 30  0001 C CNN
F 1 "GND" H 9800 6780 30  0001 C CNN
F 2 "" H 9800 6850 60  0000 C CNN
F 3 "" H 9800 6850 60  0000 C CNN
	1    9800 6850
	0    1    1    0   
$EndComp
Text Label 4300 6600 0    60   ~ 0
MR
Text Label 6550 6600 2    60   ~ 0
MR
Text Label 5400 4600 2    60   ~ 0
MR
$Comp
L GND #PWR042
U 1 1 55B68DDD
P 2650 2700
F 0 "#PWR042" H 2650 2700 30  0001 C CNN
F 1 "GND" H 2650 2630 30  0001 C CNN
F 2 "" H 2650 2700 60  0000 C CNN
F 3 "" H 2650 2700 60  0000 C CNN
	1    2650 2700
	0    1    1    0   
$EndComp
Text Label 2650 2600 2    60   ~ 0
~CPMR
Text Label 4300 7300 0    60   ~ 0
~CPMR
$Comp
L GND #PWR043
U 1 1 55BD089A
P 3350 2750
F 0 "#PWR043" H 3350 2750 30  0001 C CNN
F 1 "GND" H 3350 2680 30  0001 C CNN
F 2 "" H 3350 2750 60  0000 C CNN
F 3 "" H 3350 2750 60  0000 C CNN
	1    3350 2750
	1    0    0    -1  
$EndComp
$Comp
L CONN_9 P6
U 1 1 55BF1030
P 1250 2050
F 0 "P6" V 1200 2050 60  0000 C CNN
F 1 "USB" V 1300 2050 60  0000 C CNN
F 2 "" H 1250 2050 60  0000 C CNN
F 3 "" H 1250 2050 60  0000 C CNN
	1    1250 2050
	1    0    0    -1  
$EndComp
NoConn ~ 900  1650
NoConn ~ 900  1950
Text Label 900  1750 2    60   ~ 0
D-
Text Label 900  1850 2    60   ~ 0
D+
$Comp
L GND #PWR044
U 1 1 55BF10BD
P 900 2050
F 0 "#PWR044" H 900 2050 30  0001 C CNN
F 1 "GND" H 900 1980 30  0001 C CNN
F 2 "" H 900 2050 60  0000 C CNN
F 3 "" H 900 2050 60  0000 C CNN
	1    900  2050
	0    1    1    0   
$EndComp
NoConn ~ 900  2150
NoConn ~ 900  2250
NoConn ~ 900  2350
NoConn ~ 900  2450
$Comp
L CONN_8X2 P4
U 1 1 55BF3179
P 1250 7300
F 0 "P4" H 1250 7750 60  0000 C CNN
F 1 "USER" V 1250 7300 50  0000 C CNN
F 2 "" H 1250 7300 60  0000 C CNN
F 3 "" H 1250 7300 60  0000 C CNN
	1    1250 7300
	1    0    0    -1  
$EndComp
NoConn ~ 900  5100
Text Notes 2850 800  0    60   ~ 0
8x8 Crosspoint Switch
Text Notes 5850 800  0    60   ~ 0
Reset Signal for Counters
Text Notes 8950 800  0    60   ~ 0
Negative power supply
Text Notes 900  800  0    60   ~ 0
Connectors
Text Notes 5350 2900 0    60   ~ 0
Counters for serial Matrix scan\n
Text Notes 7600 2350 0    60   ~ 0
Matrix scan multiplexers
Text Notes 6250 5950 0    60   ~ 0
Reset/Bootloader Buttons
Text Notes 3000 3250 0    60   ~ 0
Microprocessor
Text Notes 10200 6400 0    60   ~ 0
Unused gates
Text Notes 10200 4500 0    60   ~ 0
Power flags
Text Notes 10050 5400 0    60   ~ 0
Stabilizing caps
Text Label 900  2950 2    60   ~ 0
EXTVCC
$Comp
L JUMPER JP1
U 1 1 55BF8D49
P 2100 3150
F 0 "JP1" H 2100 3300 60  0000 C CNN
F 1 "JUMPER" H 2100 3070 40  0000 C CNN
F 2 "~" H 2100 3150 60  0000 C CNN
F 3 "~" H 2100 3150 60  0000 C CNN
	1    2100 3150
	1    0    0    -1  
$EndComp
Text Label 1800 3150 2    60   ~ 0
EXTVCC
$EndSCHEMATC
