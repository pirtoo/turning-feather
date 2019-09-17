EESchema Schematic File Version 4
LIBS:turning-feather-external-cache
EELAYER 30 0
EELAYER END
$Descr User 11980 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Switch:SW_Push_DPDT SW2
U 1 1 5D6019C5
P 4050 1950
F 0 "SW2" H 4050 2435 50  0000 C CNN
F 1 "SW_Push_DPDT" H 4050 2344 50  0000 C CNN
F 2 "" H 4050 2150 50  0001 C CNN
F 3 "~" H 4050 2150 50  0001 C CNN
	1    4050 1950
	1    0    0    -1  
$EndComp
$Comp
L Device:Buzzer BZ1
U 1 1 5D619AD2
P 9500 1500
F 0 "BZ1" H 9652 1529 50  0000 L CNN
F 1 "Buzzer" H 9652 1438 50  0000 L CNN
F 2 "" V 9475 1600 50  0001 C CNN
F 3 "~" V 9475 1600 50  0001 C CNN
	1    9500 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 1500 9500 1400
$Comp
L Connector:USB_B_Micro J6
U 1 1 5D63E66A
P 4600 3150
F 0 "J6" H 4657 3617 50  0000 C CNN
F 1 "micro plug -> HUZZAH32 socket" H 4657 3526 50  0000 C CNN
F 2 "" H 4750 3100 50  0001 C CNN
F 3 "~" H 4750 3100 50  0001 C CNN
	1    4600 3150
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J3
U 1 1 5D656D28
P 8650 1750
F 0 "J3" H 8730 1792 50  0000 L CNN
F 1 "In to J3" H 8730 1701 50  0000 L CNN
F 2 "" H 8650 1750 50  0001 C CNN
F 3 "~" H 8650 1750 50  0001 C CNN
	1    8650 1750
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 5D65A636
P 7250 1750
F 0 "SW3" H 7250 2035 50  0000 C CNN
F 1 "Start/Stop" H 7250 1944 50  0000 C CNN
F 2 "" H 7250 1950 50  0001 C CNN
F 3 "~" H 7250 1950 50  0001 C CNN
	1    7250 1750
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW4
U 1 1 5D65AD7C
P 7900 1750
F 0 "SW4" H 7900 2035 50  0000 C CNN
F 1 "Face/Away" H 7900 1944 50  0000 C CNN
F 2 "" H 7900 1950 50  0001 C CNN
F 3 "~" H 7900 1950 50  0001 C CNN
	1    7900 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 1400 8450 1400
Wire Wire Line
	8450 1400 8450 1650
Wire Wire Line
	9400 1600 9400 2150
Wire Wire Line
	7700 1750 7600 1750
Connection ~ 7600 1750
Wire Wire Line
	7600 1750 7450 1750
$Comp
L Device:R_Small R4
U 1 1 5D67BEF3
P 7050 1900
F 0 "R4" H 7109 1946 50  0000 L CNN
F 1 "2.2kR" H 7109 1855 50  0000 L CNN
F 2 "" H 7050 1900 50  0001 C CNN
F 3 "~" H 7050 1900 50  0001 C CNN
	1    7050 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 1750 7050 1800
$Comp
L Connector:USB_B_Micro J5
U 1 1 5D682A96
P 3450 3150
F 0 "J5" H 3507 3617 50  0000 C CNN
F 1 "Case micro socket" H 3507 3526 50  0000 C CNN
F 2 "" H 3600 3100 50  0001 C CNN
F 3 "~" H 3600 3100 50  0001 C CNN
	1    3450 3150
	1    0    0    -1  
$EndComp
$Comp
L Connector:RJ45 J4
U 1 1 5D68C4B6
P 2300 2050
F 0 "J4" H 2357 2717 50  0000 C CNN
F 1 "RJ45 to targets" H 2357 2626 50  0000 C CNN
F 2 "" V 2300 2075 50  0001 C CNN
F 3 "~" V 2300 2075 50  0001 C CNN
	1    2300 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 3550 3450 3700
Wire Wire Line
	3450 3700 4600 3700
Wire Wire Line
	4600 3700 4600 3550
Wire Wire Line
	3350 3550 3350 3800
Wire Wire Line
	3350 3800 4700 3800
Wire Wire Line
	4700 3800 4700 3550
Wire Wire Line
	3750 3350 4300 3350
Wire Wire Line
	4300 3250 3750 3250
Wire Wire Line
	3750 3150 4300 3150
$Comp
L Connector_Generic:Conn_01x04 J4
U 1 1 5D6CB12A
P 5200 1950
F 0 "J4" H 5280 1942 50  0000 L CNN
F 1 "In to J2" H 5280 1851 50  0000 L CNN
F 2 "" H 5200 1950 50  0001 C CNN
F 3 "~" H 5200 1950 50  0001 C CNN
	1    5200 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 1350 5000 1350
Wire Wire Line
	5000 1350 5000 1850
Wire Wire Line
	4650 2150 4650 1850
Wire Wire Line
	4650 1850 4250 1850
Wire Wire Line
	2950 2450 2950 2050
Wire Wire Line
	2950 2050 2700 2050
Wire Wire Line
	2700 1950 3050 1950
Wire Wire Line
	3050 1950 3050 2350
Wire Wire Line
	4650 2150 5000 2150
Wire Wire Line
	5000 2050 4900 2050
Wire Wire Line
	4900 2050 4900 2450
Wire Wire Line
	4900 2450 2950 2450
Wire Wire Line
	3050 2350 4800 2350
Wire Wire Line
	4800 2350 4800 1950
Wire Wire Line
	4800 1950 5000 1950
Wire Wire Line
	3750 2950 3850 2950
Wire Wire Line
	3850 2950 3850 2150
Wire Wire Line
	4250 2050 4300 2050
Wire Wire Line
	4300 2050 4300 2950
Wire Wire Line
	3550 1750 3850 1750
Wire Wire Line
	3550 1850 3550 1750
Wire Wire Line
	2700 1850 3550 1850
Wire Wire Line
	2700 1650 2800 1650
Wire Wire Line
	2950 1650 2950 1350
Wire Wire Line
	2700 1750 2800 1750
Wire Wire Line
	2800 1750 2800 1650
Connection ~ 2800 1650
Wire Wire Line
	2800 1650 2950 1650
Wire Wire Line
	2800 1750 2800 2150
Wire Wire Line
	2800 2150 2700 2150
Connection ~ 2800 1750
Wire Wire Line
	7050 2150 7600 2150
Wire Wire Line
	7050 2000 7050 2150
Wire Wire Line
	7600 1750 7600 2150
Connection ~ 7600 2150
Wire Wire Line
	7600 2150 8450 2150
Wire Wire Line
	8450 1850 8450 2150
Wire Wire Line
	9400 2150 8450 2150
Connection ~ 8450 2150
Wire Wire Line
	8450 1750 8100 1750
Text Notes 2550 1200 0    50   ~ 0
This layout can be used if the board-mounted RJ45 is not used\nand board J1 not used.\nJumper pins 1 and 2 of main board J1.\nIf you wish to use a battery on the HUZZAH32 you will need to\nconnect up board J1 and J2.
$Comp
L Switch:SW_Push_DPDT SW?
U 1 1 5D820AF6
P 4100 5050
F 0 "SW?" H 4100 5535 50  0000 C CNN
F 1 "SW_Push_DPDT" H 4100 5444 50  0000 C CNN
F 2 "" H 4100 5250 50  0001 C CNN
F 3 "~" H 4100 5250 50  0001 C CNN
	1    4100 5050
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_B_Micro J?
U 1 1 5D820AFC
P 4650 6250
F 0 "J?" H 4707 6717 50  0000 C CNN
F 1 "micro plug -> HUZZAH32 socket" H 4250 6600 50  0000 C CNN
F 2 "" H 4800 6200 50  0001 C CNN
F 3 "~" H 4800 6200 50  0001 C CNN
	1    4650 6250
	-1   0    0    -1  
$EndComp
$Comp
L Connector:USB_B_Micro J?
U 1 1 5D820B02
P 3500 6250
F 0 "J?" H 3557 6717 50  0000 C CNN
F 1 "Case micro socket" H 3350 6600 50  0000 C CNN
F 2 "" H 3650 6200 50  0001 C CNN
F 3 "~" H 3650 6200 50  0001 C CNN
	1    3500 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 6650 3500 6800
Wire Wire Line
	3500 6800 4650 6800
Wire Wire Line
	4650 6800 4650 6650
Wire Wire Line
	3400 6650 3400 6900
Wire Wire Line
	3400 6900 4750 6900
Wire Wire Line
	4750 6900 4750 6650
Wire Wire Line
	3800 6450 4350 6450
Wire Wire Line
	4350 6350 3800 6350
Wire Wire Line
	3800 6250 4350 6250
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 5D820B11
P 5250 5050
F 0 "J?" H 5330 5042 50  0000 L CNN
F 1 "In to J2" H 5330 4951 50  0000 L CNN
F 2 "" H 5250 5050 50  0001 C CNN
F 3 "~" H 5250 5050 50  0001 C CNN
	1    5250 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 5250 5050 5250
$Comp
L Device:D_Schottky_Small D1
U 1 1 5D82304C
P 4100 6050
F 0 "D1" H 4100 5845 50  0000 C CNN
F 1 "1N5817" H 4100 5936 50  0000 C CNN
F 2 "" V 4100 6050 50  0001 C CNN
F 3 "~" V 4100 6050 50  0001 C CNN
	1    4100 6050
	-1   0    0    1   
$EndComp
Wire Wire Line
	3800 6050 4000 6050
Wire Wire Line
	4200 6050 4350 6050
Wire Wire Line
	4700 5250 4700 5550
Wire Wire Line
	4300 5150 5050 5150
Wire Wire Line
	4700 5050 4700 4950
Wire Wire Line
	4700 5050 5050 5050
Wire Wire Line
	4300 4950 4700 4950
Wire Wire Line
	5050 4950 5050 4450
Wire Wire Line
	5050 4450 3700 4450
Wire Wire Line
	3700 4450 3700 4850
Wire Wire Line
	3700 4850 3900 4850
Wire Wire Line
	3900 5250 3700 5250
Wire Wire Line
	3700 5250 3700 5550
Wire Wire Line
	3700 5550 4700 5550
Text Notes 3050 4400 0    50   ~ 0
This layout is used if you use the board-mounted RJ45\nconnector. Suppy voltage from USB should have a diode\ninline or you risk back-feeding 5V when 12V is supplied.\nThis supports a battery connected to the HUZZAH32.
$EndSCHEMATC
