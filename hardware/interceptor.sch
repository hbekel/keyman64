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
LIBS:interceptor-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Programmable C64 Keyboard Interceptor and Controller"
Date "30 jul 2015"
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
P 1250 4650
F 0 "P3" V 1200 4650 60  0000 C CNN
F 1 "KEYBOARD" V 1300 4650 60  0000 C CNN
F 2 "" H 1250 4650 60  0000 C CNN
F 3 "" H 1250 4650 60  0000 C CNN
	1    1250 4650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 558199D0
P 900 3700
F 0 "#PWR01" H 900 3700 30  0001 C CNN
F 1 "GND" H 900 3630 30  0001 C CNN
F 2 "" H 900 3700 60  0000 C CNN
F 3 "" H 900 3700 60  0000 C CNN
	1    900  3700
	0    1    1    0   
$EndComp
NoConn ~ 900  3800
Text Label 900  3900 2    60   ~ 0
~RESTORE
Text Label 900  4000 2    60   ~ 0
VCC
$Comp
L CONN_20 P2
U 1 1 558432CB
P 1250 2450
F 0 "P2" V 1200 2450 60  0000 C CNN
F 1 "CIA1" V 1300 2450 60  0000 C CNN
F 2 "" H 1250 2450 60  0000 C CNN
F 3 "" H 1250 2450 60  0000 C CNN
	1    1250 2450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 558432D1
P 900 1500
F 0 "#PWR02" H 900 1500 30  0001 C CNN
F 1 "GND" H 900 1430 30  0001 C CNN
F 2 "" H 900 1500 60  0000 C CNN
F 3 "" H 900 1500 60  0000 C CNN
	1    900  1500
	0    1    1    0   
$EndComp
NoConn ~ 900  1600
Text Label 900  1700 2    60   ~ 0
~RESTORE
Text Label 900  1900 2    60   ~ 0
B3
Text Label 900  2600 2    60   ~ 0
B0
Text Label 900  2500 2    60   ~ 0
B1
Text Label 900  2400 2    60   ~ 0
B2
Text Label 900  2200 2    60   ~ 0
B4
Text Label 900  2100 2    60   ~ 0
B5
Text Label 900  2000 2    60   ~ 0
B6
Text Label 900  2300 2    60   ~ 0
B7
Text Label 900  2700 2    60   ~ 0
A0
Text Label 900  3300 2    60   ~ 0
A1
Text Label 900  3200 2    60   ~ 0
A2
Text Label 900  3100 2    60   ~ 0
A3
Text Label 900  3000 2    60   ~ 0
A4
Text Label 900  2900 2    60   ~ 0
A5
Text Label 900  2800 2    60   ~ 0
A6
Text Label 900  3400 2    60   ~ 0
A7
Text Label 900  1800 2    60   ~ 0
VCC
Text Label 4050 1000 0    60   ~ 0
A0
Text Label 4050 1100 0    60   ~ 0
A1
Text Label 4050 1200 0    60   ~ 0
A2
Text Label 4050 1300 0    60   ~ 0
A3
Text Label 4050 1400 0    60   ~ 0
A4
Text Label 4050 1500 0    60   ~ 0
A5
Text Label 4050 1600 0    60   ~ 0
A6
Text Label 4050 1700 0    60   ~ 0
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
Text Label 4050 1900 0    60   ~ 0
B0
Text Label 4050 2000 0    60   ~ 0
B1
Text Label 4050 2100 0    60   ~ 0
B2
Text Label 4050 2200 0    60   ~ 0
B3
Text Label 4050 2300 0    60   ~ 0
B4
Text Label 4050 2400 0    60   ~ 0
B5
Text Label 4050 2500 0    60   ~ 0
B6
Text Label 4050 2600 0    60   ~ 0
B7
Text Label 4300 6900 0    60   ~ 0
MC
Text Label 4300 7000 0    60   ~ 0
MD
Text Label 4300 7100 0    60   ~ 0
CS
Text Label 4300 7200 0    60   ~ 0
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
P 6350 7500
F 0 "#PWR03" H 6350 7600 30  0001 C CNN
F 1 "VCC" H 6350 7600 30  0000 C CNN
F 2 "" H 6350 7500 60  0000 C CNN
F 3 "" H 6350 7500 60  0000 C CNN
	1    6350 7500
	-1   0    0    1   
$EndComp
$Comp
L PWR_FLAG #FLG04
U 1 1 55A7672F
P 6350 7400
F 0 "#FLG04" H 6350 7495 30  0001 C CNN
F 1 "PWR_FLAG" H 6350 7580 30  0000 C CNN
F 2 "" H 6350 7400 60  0000 C CNN
F 3 "" H 6350 7400 60  0000 C CNN
	1    6350 7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 7400 6350 7500
$Comp
L PWR_FLAG #FLG05
U 1 1 55A767C3
P 6650 7400
F 0 "#FLG05" H 6650 7495 30  0001 C CNN
F 1 "PWR_FLAG" H 6650 7580 30  0000 C CNN
F 2 "" H 6650 7400 60  0000 C CNN
F 3 "" H 6650 7400 60  0000 C CNN
	1    6650 7400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 55A767D2
P 6650 7550
F 0 "#PWR06" H 6650 7550 30  0001 C CNN
F 1 "GND" H 6650 7480 30  0001 C CNN
F 2 "" H 6650 7550 60  0000 C CNN
F 3 "" H 6650 7550 60  0000 C CNN
	1    6650 7550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 7400 6650 7550
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
P 6500 3300
F 0 "U3" H 6550 3750 60  0000 C CNN
F 1 "4520" H 6700 2950 60  0000 C CNN
F 2 "" H 6500 3300 60  0000 C CNN
F 3 "" H 6500 3300 60  0000 C CNN
	1    6500 3300
	1    0    0    -1  
$EndComp
$Comp
L 4520 U3
U 2 1 55A767FE
P 6500 4350
F 0 "U3" H 6550 4800 60  0000 C CNN
F 1 "4520" H 6700 4000 60  0000 C CNN
F 2 "" H 6500 4350 60  0000 C CNN
F 3 "" H 6500 4350 60  0000 C CNN
	2    6500 4350
	1    0    0    -1  
$EndComp
Text Label 5900 3150 2    60   ~ 0
MC
Text Label 7100 2950 0    60   ~ 0
CS0
Text Label 7100 3050 0    60   ~ 0
CS1
Text Label 7100 3150 0    60   ~ 0
CS2
Text Label 7100 4000 0    60   ~ 0
RS0
Text Label 7100 4100 0    60   ~ 0
RS1
Text Label 7100 4200 0    60   ~ 0
RS2
NoConn ~ 7100 4300
$Comp
L 7400 U1
U 2 1 55A76CFC
P 5650 950
F 0 "U1" H 5650 1000 60  0000 C CNN
F 1 "7400" H 5650 850 60  0000 C CNN
F 2 "~" H 5650 950 60  0000 C CNN
F 3 "~" H 5650 950 60  0000 C CNN
	2    5650 950 
	1    0    0    -1  
$EndComp
$Comp
L 7400 U1
U 3 1 55A76D17
P 5650 1600
F 0 "U1" H 5650 1650 60  0000 C CNN
F 1 "7400" H 5650 1500 60  0000 C CNN
F 2 "~" H 5650 1600 60  0000 C CNN
F 3 "~" H 5650 1600 60  0000 C CNN
	3    5650 1600
	1    0    0    -1  
$EndComp
$Comp
L 7400 U1
U 4 1 55A76D32
P 7000 1300
F 0 "U1" H 7000 1350 60  0000 C CNN
F 1 "7400" H 7000 1200 60  0000 C CNN
F 2 "~" H 7000 1300 60  0000 C CNN
F 3 "~" H 7000 1300 60  0000 C CNN
	4    7000 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 1200 6250 1200
Wire Wire Line
	6250 1200 6250 950 
Wire Wire Line
	6250 1600 6250 1400
Wire Wire Line
	6250 1400 6400 1400
Wire Wire Line
	5050 850  5050 1050
Wire Wire Line
	5050 1500 5050 1700
Wire Wire Line
	5050 950  4950 950 
Connection ~ 5050 950 
Text Label 4950 950  2    60   ~ 0
MR
Text Label 7600 1300 0    60   ~ 0
CRES
Text Label 5900 3250 2    60   ~ 0
CRES
Text Label 7100 3250 0    60   ~ 0
RCK
Text Label 5900 4200 2    60   ~ 0
RCK
$Comp
L GND #PWR09
U 1 1 55A76DEF
P 5900 3350
F 0 "#PWR09" H 5900 3350 30  0001 C CNN
F 1 "GND" H 5900 3280 30  0001 C CNN
F 2 "" H 5900 3350 60  0000 C CNN
F 3 "" H 5900 3350 60  0000 C CNN
	1    5900 3350
	0    1    1    0   
$EndComp
$Comp
L GND #PWR010
U 1 1 55A76DFE
P 5900 4400
F 0 "#PWR010" H 5900 4400 30  0001 C CNN
F 1 "GND" H 5900 4330 30  0001 C CNN
F 2 "" H 5900 4400 60  0000 C CNN
F 3 "" H 5900 4400 60  0000 C CNN
	1    5900 4400
	0    1    1    0   
$EndComp
Text Label 4900 1600 2    60   ~ 0
RCK
$Comp
L 4051 U4
U 1 1 55A76EFE
P 8400 3400
F 0 "U4" H 8500 3400 60  0000 C CNN
F 1 "4051" H 8500 3200 60  0000 C CNN
F 2 "" H 8400 3400 60  0000 C CNN
F 3 "" H 8400 3400 60  0000 C CNN
	1    8400 3400
	1    0    0    -1  
$EndComp
Text Label 7700 3800 2    60   ~ 0
CS0
Text Label 7700 3900 2    60   ~ 0
CS1
Text Label 7700 4000 2    60   ~ 0
CS2
$Comp
L 4051 U5
U 1 1 55A76F63
P 8400 4950
F 0 "U5" H 8500 4950 60  0000 C CNN
F 1 "4051" H 8500 4750 60  0000 C CNN
F 2 "" H 8400 4950 60  0000 C CNN
F 3 "" H 8400 4950 60  0000 C CNN
	1    8400 4950
	1    0    0    -1  
$EndComp
Text Label 7700 5350 2    60   ~ 0
RS0
Text Label 7700 5450 2    60   ~ 0
RS1
Text Label 7700 5550 2    60   ~ 0
RS2
Text Label 9100 4350 0    60   ~ 0
MD
$Comp
L GND #PWR011
U 1 1 55A77078
P 9100 2800
F 0 "#PWR011" H 9100 2800 30  0001 C CNN
F 1 "GND" H 9100 2730 30  0001 C CNN
F 2 "" H 9100 2800 60  0000 C CNN
F 3 "" H 9100 2800 60  0000 C CNN
	1    9100 2800
	0    -1   -1   0   
$EndComp
Text Label 7700 2800 2    60   ~ 0
C0
Text Label 7700 2900 2    60   ~ 0
C1
Text Label 7700 3000 2    60   ~ 0
C2
Text Label 7700 3100 2    60   ~ 0
C3
Text Label 7700 3200 2    60   ~ 0
C4
Text Label 7700 3300 2    60   ~ 0
C5
Text Label 7700 3400 2    60   ~ 0
C6
Text Label 7700 3500 2    60   ~ 0
C7
Text Label 7700 4350 2    60   ~ 0
R0
Text Label 7700 4450 2    60   ~ 0
R1
Text Label 7700 4550 2    60   ~ 0
R2
Text Label 7700 4650 2    60   ~ 0
R3
Text Label 7700 4750 2    60   ~ 0
R4
Text Label 7700 4850 2    60   ~ 0
R5
Text Label 7700 4950 2    60   ~ 0
R6
Text Label 7700 5050 2    60   ~ 0
R7
$Comp
L CONN_2 P1
U 1 1 55A772B0
P 1300 1150
F 0 "P1" V 1250 1150 40  0000 C CNN
F 1 "TAPEPORT" V 1350 1150 40  0000 C CNN
F 2 "" H 1300 1150 60  0000 C CNN
F 3 "" H 1300 1150 60  0000 C CNN
	1    1300 1150
	1    0    0    -1  
$EndComp
Text Label 950  1050 2    60   ~ 0
CS
Text Label 950  1250 2    60   ~ 0
CD
$Comp
L ICL7660 U2
U 1 1 55A772D4
P 9550 1400
F 0 "U2" H 9750 1800 70  0000 L CNN
F 1 "ICL7660" H 9600 950 70  0000 L CNN
F 2 "" H 9550 1400 60  0000 C CNN
F 3 "" H 9550 1400 60  0000 C CNN
	1    9550 1400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR012
U 1 1 55A772ED
P 9500 2050
F 0 "#PWR012" H 9500 2050 30  0001 C CNN
F 1 "GND" H 9500 1980 30  0001 C CNN
F 2 "" H 9500 2050 60  0000 C CNN
F 3 "" H 9500 2050 60  0000 C CNN
	1    9500 2050
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR013
U 1 1 55A772FC
P 9500 750
F 0 "#PWR013" H 9500 850 30  0001 C CNN
F 1 "VCC" H 9500 850 30  0000 C CNN
F 2 "" H 9500 750 60  0000 C CNN
F 3 "" H 9500 750 60  0000 C CNN
	1    9500 750 
	1    0    0    -1  
$EndComp
$Comp
L CP1 C4
U 1 1 55A7730B
P 8400 1200
F 0 "C4" H 8450 1300 50  0000 L CNN
F 1 "10uF" H 8450 1100 50  0000 L CNN
F 2 "~" H 8400 1200 60  0000 C CNN
F 3 "~" H 8400 1200 60  0000 C CNN
	1    8400 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 1350 8650 1350
Wire Wire Line
	8650 1350 8650 1400
Wire Wire Line
	8650 1400 8400 1400
Wire Wire Line
	8700 1150 8650 1150
Wire Wire Line
	8650 1150 8650 1000
Wire Wire Line
	8650 1000 8400 1000
NoConn ~ 8700 1550
NoConn ~ 10400 1550
$Comp
L CP1 C5
U 1 1 55A77345
P 10650 1450
F 0 "C5" H 10700 1550 50  0000 L CNN
F 1 "10uF" H 10700 1350 50  0000 L CNN
F 2 "~" H 10650 1450 60  0000 C CNN
F 3 "~" H 10650 1450 60  0000 C CNN
	1    10650 1450
	-1   0    0    1   
$EndComp
Wire Wire Line
	10400 1250 10900 1250
Connection ~ 10650 1250
Text Label 10900 1250 0    60   ~ 0
VEE
$Comp
L GND #PWR014
U 1 1 55A77382
P 10650 1650
F 0 "#PWR014" H 10650 1650 30  0001 C CNN
F 1 "GND" H 10650 1580 30  0001 C CNN
F 2 "" H 10650 1650 60  0000 C CNN
F 3 "" H 10650 1650 60  0000 C CNN
	1    10650 1650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 55A774B7
P 7700 5250
F 0 "#PWR015" H 7700 5250 30  0001 C CNN
F 1 "GND" H 7700 5180 30  0001 C CNN
F 2 "" H 7700 5250 60  0000 C CNN
F 3 "" H 7700 5250 60  0000 C CNN
	1    7700 5250
	0    1    1    0   
$EndComp
$Comp
L GND #PWR016
U 1 1 55A774BD
P 7700 3700
F 0 "#PWR016" H 7700 3700 30  0001 C CNN
F 1 "GND" H 7700 3630 30  0001 C CNN
F 2 "" H 7700 3700 60  0000 C CNN
F 3 "" H 7700 3700 60  0000 C CNN
	1    7700 3700
	0    1    1    0   
$EndComp
Wire Wire Line
	4900 1600 5050 1600
Connection ~ 5050 1600
Text Label 900  4100 2    60   ~ 0
R3
Text Label 900  4800 2    60   ~ 0
R0
Text Label 900  4700 2    60   ~ 0
R1
Text Label 900  4600 2    60   ~ 0
R2
Text Label 900  4400 2    60   ~ 0
R4
Text Label 900  4300 2    60   ~ 0
R5
Text Label 900  4200 2    60   ~ 0
R6
Text Label 900  4500 2    60   ~ 0
R7
Text Label 900  4900 2    60   ~ 0
C0
Text Label 900  5500 2    60   ~ 0
C1
Text Label 900  5400 2    60   ~ 0
C2
Text Label 900  5300 2    60   ~ 0
C3
Text Label 900  5200 2    60   ~ 0
C4
Text Label 900  5100 2    60   ~ 0
C5
Text Label 900  5000 2    60   ~ 0
C6
Text Label 900  5600 2    60   ~ 0
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
P 6350 2900
F 0 "#PWR019" H 6350 3000 30  0001 C CNN
F 1 "VCC" H 6350 3000 30  0000 C CNN
F 2 "" H 6350 2900 60  0000 C CNN
F 3 "" H 6350 2900 60  0000 C CNN
	1    6350 2900
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR020
U 1 1 55A77A8E
P 6350 3950
F 0 "#PWR020" H 6350 4050 30  0001 C CNN
F 1 "VCC" H 6350 4050 30  0000 C CNN
F 2 "" H 6350 3950 60  0000 C CNN
F 3 "" H 6350 3950 60  0000 C CNN
	1    6350 3950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR021
U 1 1 55A77A94
P 8400 2750
F 0 "#PWR021" H 8400 2850 30  0001 C CNN
F 1 "VCC" H 8400 2850 30  0000 C CNN
F 2 "" H 8400 2750 60  0000 C CNN
F 3 "" H 8400 2750 60  0000 C CNN
	1    8400 2750
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR022
U 1 1 55A77A9A
P 8400 4300
F 0 "#PWR022" H 8400 4400 30  0001 C CNN
F 1 "VCC" H 8400 4400 30  0000 C CNN
F 2 "" H 8400 4300 60  0000 C CNN
F 3 "" H 8400 4300 60  0000 C CNN
	1    8400 4300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR023
U 1 1 55A77AA2
P 6350 3600
F 0 "#PWR023" H 6350 3600 30  0001 C CNN
F 1 "GND" H 6350 3530 30  0001 C CNN
F 2 "" H 6350 3600 60  0000 C CNN
F 3 "" H 6350 3600 60  0000 C CNN
	1    6350 3600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR024
U 1 1 55A77AAF
P 6350 4650
F 0 "#PWR024" H 6350 4650 30  0001 C CNN
F 1 "GND" H 6350 4580 30  0001 C CNN
F 2 "" H 6350 4650 60  0000 C CNN
F 3 "" H 6350 4650 60  0000 C CNN
	1    6350 4650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR025
U 1 1 55A77AB5
P 8400 4050
F 0 "#PWR025" H 8400 4050 30  0001 C CNN
F 1 "GND" H 8400 3980 30  0001 C CNN
F 2 "" H 8400 4050 60  0000 C CNN
F 3 "" H 8400 4050 60  0000 C CNN
	1    8400 4050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR026
U 1 1 55A77ABB
P 8400 5600
F 0 "#PWR026" H 8400 5600 30  0001 C CNN
F 1 "GND" H 8400 5530 30  0001 C CNN
F 2 "" H 8400 5600 60  0000 C CNN
F 3 "" H 8400 5600 60  0000 C CNN
	1    8400 5600
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 55A77F14
P 10650 2600
F 0 "C7" H 10650 2700 40  0000 L CNN
F 1 "100nF" H 10656 2515 40  0000 L CNN
F 2 "~" H 10688 2450 30  0000 C CNN
F 3 "~" H 10650 2600 60  0000 C CNN
	1    10650 2600
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR027
U 1 1 55A77F23
P 10650 2400
F 0 "#PWR027" H 10650 2500 30  0001 C CNN
F 1 "VCC" H 10650 2500 30  0000 C CNN
F 2 "" H 10650 2400 60  0000 C CNN
F 3 "" H 10650 2400 60  0000 C CNN
	1    10650 2400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR028
U 1 1 55A77F32
P 10650 2800
F 0 "#PWR028" H 10650 2800 30  0001 C CNN
F 1 "GND" H 10650 2730 30  0001 C CNN
F 2 "" H 10650 2800 60  0000 C CNN
F 3 "" H 10650 2800 60  0000 C CNN
	1    10650 2800
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 55A77F5A
P 10900 2600
F 0 "C8" H 10900 2700 40  0000 L CNN
F 1 "100nF" H 10906 2515 40  0000 L CNN
F 2 "~" H 10938 2450 30  0000 C CNN
F 3 "~" H 10900 2600 60  0000 C CNN
	1    10900 2600
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR029
U 1 1 55A77F60
P 10900 2400
F 0 "#PWR029" H 10900 2500 30  0001 C CNN
F 1 "VCC" H 10900 2500 30  0000 C CNN
F 2 "" H 10900 2400 60  0000 C CNN
F 3 "" H 10900 2400 60  0000 C CNN
	1    10900 2400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR030
U 1 1 55A77F66
P 10900 2800
F 0 "#PWR030" H 10900 2800 30  0001 C CNN
F 1 "GND" H 10900 2730 30  0001 C CNN
F 2 "" H 10900 2800 60  0000 C CNN
F 3 "" H 10900 2800 60  0000 C CNN
	1    10900 2800
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 55A77F6C
P 10650 3300
F 0 "C9" H 10650 3400 40  0000 L CNN
F 1 "100nF" H 10656 3215 40  0000 L CNN
F 2 "~" H 10688 3150 30  0000 C CNN
F 3 "~" H 10650 3300 60  0000 C CNN
	1    10650 3300
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR031
U 1 1 55A77F72
P 10650 3100
F 0 "#PWR031" H 10650 3200 30  0001 C CNN
F 1 "VCC" H 10650 3200 30  0000 C CNN
F 2 "" H 10650 3100 60  0000 C CNN
F 3 "" H 10650 3100 60  0000 C CNN
	1    10650 3100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR032
U 1 1 55A77F78
P 10650 3500
F 0 "#PWR032" H 10650 3500 30  0001 C CNN
F 1 "GND" H 10650 3430 30  0001 C CNN
F 2 "" H 10650 3500 60  0000 C CNN
F 3 "" H 10650 3500 60  0000 C CNN
	1    10650 3500
	1    0    0    -1  
$EndComp
$Comp
L C C10
U 1 1 55A77F7E
P 10900 3300
F 0 "C10" H 10900 3400 40  0000 L CNN
F 1 "100nF" H 10906 3215 40  0000 L CNN
F 2 "~" H 10938 3150 30  0000 C CNN
F 3 "~" H 10900 3300 60  0000 C CNN
	1    10900 3300
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR033
U 1 1 55A77F84
P 10900 3100
F 0 "#PWR033" H 10900 3200 30  0001 C CNN
F 1 "VCC" H 10900 3200 30  0000 C CNN
F 2 "" H 10900 3100 60  0000 C CNN
F 3 "" H 10900 3100 60  0000 C CNN
	1    10900 3100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR034
U 1 1 55A77F8A
P 10900 3500
F 0 "#PWR034" H 10900 3500 30  0001 C CNN
F 1 "GND" H 10900 3430 30  0001 C CNN
F 2 "" H 10900 3500 60  0000 C CNN
F 3 "" H 10900 3500 60  0000 C CNN
	1    10900 3500
	1    0    0    -1  
$EndComp
$Comp
L C C11
U 1 1 55A77F90
P 10650 4150
F 0 "C11" H 10650 4250 40  0000 L CNN
F 1 "100nF" H 10656 4065 40  0000 L CNN
F 2 "~" H 10688 4000 30  0000 C CNN
F 3 "~" H 10650 4150 60  0000 C CNN
	1    10650 4150
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR035
U 1 1 55A77F96
P 10650 3950
F 0 "#PWR035" H 10650 4050 30  0001 C CNN
F 1 "VCC" H 10650 4050 30  0000 C CNN
F 2 "" H 10650 3950 60  0000 C CNN
F 3 "" H 10650 3950 60  0000 C CNN
	1    10650 3950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR036
U 1 1 55A77F9C
P 10650 4350
F 0 "#PWR036" H 10650 4350 30  0001 C CNN
F 1 "GND" H 10650 4280 30  0001 C CNN
F 2 "" H 10650 4350 60  0000 C CNN
F 3 "" H 10650 4350 60  0000 C CNN
	1    10650 4350
	1    0    0    -1  
$EndComp
$Comp
L C C12
U 1 1 55A77FA2
P 10900 4150
F 0 "C12" H 10900 4250 40  0000 L CNN
F 1 "100nF" H 10906 4065 40  0000 L CNN
F 2 "~" H 10938 4000 30  0000 C CNN
F 3 "~" H 10900 4150 60  0000 C CNN
	1    10900 4150
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR037
U 1 1 55A77FA8
P 10900 3950
F 0 "#PWR037" H 10900 4050 30  0001 C CNN
F 1 "VCC" H 10900 4050 30  0000 C CNN
F 2 "" H 10900 3950 60  0000 C CNN
F 3 "" H 10900 3950 60  0000 C CNN
	1    10900 3950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR038
U 1 1 55A77FAE
P 10900 4350
F 0 "#PWR038" H 10900 4350 30  0001 C CNN
F 1 "GND" H 10900 4280 30  0001 C CNN
F 2 "" H 10900 4350 60  0000 C CNN
F 3 "" H 10900 4350 60  0000 C CNN
	1    10900 4350
	1    0    0    -1  
$EndComp
Text Label 9100 3900 0    60   ~ 0
VEE
$Comp
L GND #PWR039
U 1 1 55B35D41
P 9100 5450
F 0 "#PWR039" H 9100 5450 30  0001 C CNN
F 1 "GND" H 9100 5380 30  0001 C CNN
F 2 "" H 9100 5450 60  0000 C CNN
F 3 "" H 9100 5450 60  0000 C CNN
	1    9100 5450
	0    -1   -1   0   
$EndComp
$Comp
L CD74HC22106 U6
U 1 1 55B3756A
P 3350 1800
F 0 "U6" H 3000 2700 60  0000 C CNN
F 1 "CD74HC22106" H 3350 700 60  0000 C CNN
F 2 "" H 3300 2500 60  0000 C CNN
F 3 "" H 3300 2500 60  0000 C CNN
	1    3350 1800
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR040
U 1 1 55B37587
P 3350 950
F 0 "#PWR040" H 3350 1050 30  0001 C CNN
F 1 "VCC" H 3350 1050 30  0000 C CNN
F 2 "" H 3350 950 60  0000 C CNN
F 3 "" H 3350 950 60  0000 C CNN
	1    3350 950 
	1    0    0    -1  
$EndComp
Text Label 3350 2650 3    60   ~ 0
VEE
Text Label 2650 1000 2    60   ~ 0
CPA0
Text Label 2650 1100 2    60   ~ 0
CPA1
Text Label 2650 1200 2    60   ~ 0
CPA2
Text Label 2650 1300 2    60   ~ 0
CPA3
Text Label 2650 1400 2    60   ~ 0
CPA4
Text Label 2650 1500 2    60   ~ 0
CPA5
Text Label 2650 1950 2    60   ~ 0
~CPS
Text Label 2650 2050 2    60   ~ 0
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
L GND #PWR041
U 1 1 55B37A62
P 6200 5700
F 0 "#PWR041" H 6200 5700 30  0001 C CNN
F 1 "GND" H 6200 5630 30  0001 C CNN
F 2 "" H 6200 5700 60  0000 C CNN
F 3 "" H 6200 5700 60  0000 C CNN
	1    6200 5700
	0    -1   -1   0   
$EndComp
$Comp
L SW_PUSH SW1
U 1 1 55B37A78
P 5900 5350
F 0 "SW1" H 6050 5460 50  0000 C CNN
F 1 "RESET" H 5900 5270 50  0000 C CNN
F 2 "" H 5900 5350 60  0000 C CNN
F 3 "" H 5900 5350 60  0000 C CNN
	1    5900 5350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR042
U 1 1 55B37A96
P 6200 5350
F 0 "#PWR042" H 6200 5350 30  0001 C CNN
F 1 "GND" H 6200 5280 30  0001 C CNN
F 2 "" H 6200 5350 60  0000 C CNN
F 3 "" H 6200 5350 60  0000 C CNN
	1    6200 5350
	0    -1   -1   0   
$EndComp
Text Label 5600 5350 2    60   ~ 0
~RESET
Text Label 2300 3900 2    60   ~ 0
~RESET
$Comp
L CONN_3 K1
U 1 1 55B37C90
P 1300 700
F 0 "K1" V 1250 700 50  0000 C CNN
F 1 "USB" V 1350 700 40  0000 C CNN
F 2 "" H 1300 700 60  0000 C CNN
F 3 "" H 1300 700 60  0000 C CNN
	1    1300 700 
	1    0    0    -1  
$EndComp
Text Label 950  700  2    60   ~ 0
D+
Text Label 950  600  2    60   ~ 0
D-
$Comp
L GND #PWR043
U 1 1 55B37CAF
P 950 800
F 0 "#PWR043" H 950 800 30  0001 C CNN
F 1 "GND" H 950 730 30  0001 C CNN
F 2 "" H 950 800 60  0000 C CNN
F 3 "" H 950 800 60  0000 C CNN
	1    950  800 
	0    1    1    0   
$EndComp
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
Text Label 950  5950 2    60   ~ 0
UA0
Text Label 950  6050 2    60   ~ 0
UA1
Text Label 950  6150 2    60   ~ 0
UA2
Text Label 950  6250 2    60   ~ 0
UA3
Text Label 950  6350 2    60   ~ 0
UA4
Text Label 950  6450 2    60   ~ 0
UA5
Text Label 950  6550 2    60   ~ 0
UA6
Text Label 950  6650 2    60   ~ 0
UA7
Text Label 950  6850 2    60   ~ 0
UB0
Text Label 950  6950 2    60   ~ 0
UB1
Text Label 950  7050 2    60   ~ 0
UB2
Text Label 950  7150 2    60   ~ 0
UB3
Text Label 950  7250 2    60   ~ 0
UB4
Text Label 950  7350 2    60   ~ 0
UB5
Text Label 950  7450 2    60   ~ 0
UB6
Text Label 950  7550 2    60   ~ 0
UB7
$Comp
L C C13
U 1 1 55B38383
P 10750 4950
F 0 "C13" H 10750 5050 40  0000 L CNN
F 1 "100nF" H 10756 4865 40  0000 L CNN
F 2 "~" H 10788 4800 30  0000 C CNN
F 3 "~" H 10750 4950 60  0000 C CNN
	1    10750 4950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR044
U 1 1 55B38389
P 10750 4750
F 0 "#PWR044" H 10750 4850 30  0001 C CNN
F 1 "VCC" H 10750 4850 30  0000 C CNN
F 2 "" H 10750 4750 60  0000 C CNN
F 3 "" H 10750 4750 60  0000 C CNN
	1    10750 4750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR045
U 1 1 55B3838F
P 10750 5150
F 0 "#PWR045" H 10750 5150 30  0001 C CNN
F 1 "GND" H 10750 5080 30  0001 C CNN
F 2 "" H 10750 5150 60  0000 C CNN
F 3 "" H 10750 5150 60  0000 C CNN
	1    10750 5150
	1    0    0    -1  
$EndComp
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
L VCC #PWR046
U 1 1 55B387FF
P 5200 6200
F 0 "#PWR046" H 5200 6300 30  0001 C CNN
F 1 "VCC" H 5200 6300 30  0000 C CNN
F 2 "" H 5200 6200 60  0000 C CNN
F 3 "" H 5200 6200 60  0000 C CNN
	1    5200 6200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR047
U 1 1 55B38813
P 5200 7200
F 0 "#PWR047" H 5200 7200 30  0001 C CNN
F 1 "GND" H 5200 7130 30  0001 C CNN
F 2 "" H 5200 7200 60  0000 C CNN
F 3 "" H 5200 7200 60  0000 C CNN
	1    5200 7200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR048
U 1 1 55B38825
P 4900 7100
F 0 "#PWR048" H 4900 7100 30  0001 C CNN
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
P 5900 5700
F 0 "SW2" H 6050 5810 50  0000 C CNN
F 1 "BOOTLOADER" H 5900 5620 50  0000 C CNN
F 2 "" H 5900 5700 60  0000 C CNN
F 3 "" H 5900 5700 60  0000 C CNN
	1    5900 5700
	1    0    0    -1  
$EndComp
$Comp
L 7400 U1
U 1 1 55A76CA9
P 10500 6100
F 0 "U1" H 10500 6150 60  0000 C CNN
F 1 "7400" H 10500 6000 60  0000 C CNN
F 2 "~" H 10500 6100 60  0000 C CNN
F 3 "~" H 10500 6100 60  0000 C CNN
	1    10500 6100
	1    0    0    -1  
$EndComp
NoConn ~ 11100 6100
$Comp
L GND #PWR049
U 1 1 55B4B074
P 9900 6000
F 0 "#PWR049" H 9900 6000 30  0001 C CNN
F 1 "GND" H 9900 5930 30  0001 C CNN
F 2 "" H 9900 6000 60  0000 C CNN
F 3 "" H 9900 6000 60  0000 C CNN
	1    9900 6000
	0    1    1    0   
$EndComp
$Comp
L GND #PWR050
U 1 1 55B4B08B
P 9900 6200
F 0 "#PWR050" H 9900 6200 30  0001 C CNN
F 1 "GND" H 9900 6130 30  0001 C CNN
F 2 "" H 9900 6200 60  0000 C CNN
F 3 "" H 9900 6200 60  0000 C CNN
	1    9900 6200
	0    1    1    0   
$EndComp
Text Label 4300 6600 0    60   ~ 0
MR
Text Label 5600 5700 2    60   ~ 0
MR
Text Label 5900 4300 2    60   ~ 0
MR
$Comp
L GND #PWR051
U 1 1 55B68DDD
P 2650 2600
F 0 "#PWR051" H 2650 2600 30  0001 C CNN
F 1 "GND" H 2650 2530 30  0001 C CNN
F 2 "" H 2650 2600 60  0000 C CNN
F 3 "" H 2650 2600 60  0000 C CNN
	1    2650 2600
	0    1    1    0   
$EndComp
Text Label 2650 2500 2    60   ~ 0
~CPMR
Text Label 4300 7300 0    60   ~ 0
~CPMR
$Comp
L CONN_8 P4
U 1 1 55BA1527
P 1300 6300
F 0 "P4" V 1250 6300 60  0000 C CNN
F 1 "USER A" V 1350 6300 60  0000 C CNN
F 2 "" H 1300 6300 60  0000 C CNN
F 3 "" H 1300 6300 60  0000 C CNN
	1    1300 6300
	1    0    0    -1  
$EndComp
$Comp
L CONN_8 P5
U 1 1 55BA1542
P 1300 7200
F 0 "P5" V 1250 7200 60  0000 C CNN
F 1 "USER B" V 1350 7200 60  0000 C CNN
F 2 "" H 1300 7200 60  0000 C CNN
F 3 "" H 1300 7200 60  0000 C CNN
	1    1300 7200
	1    0    0    -1  
$EndComp
$EndSCHEMATC
