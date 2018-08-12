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
LIBS:Borgel Custom
LIBS:switches
LIBS:Sympetrum-V3 FF3-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 7 7
Title "Sympetrum-V3"
Date ""
Rev "FF3"
Comp "Ancillary Amalgamation"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L C_Small C701
U 1 1 589ACFA6
P 4050 4000
F 0 "C701" H 4060 4070 50  0000 L CNN
F 1 "4.7uF" H 4100 3900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4050 4000 50  0001 C CNN
F 3 "" H 4050 4000 50  0001 C CNN
	1    4050 4000
	1    0    0    -1  
$EndComp
$Comp
L C_Small C702
U 1 1 589ACFA7
P 9000 4000
F 0 "C702" H 9010 4070 50  0000 L CNN
F 1 "4.7uF" H 9050 3900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9000 4000 50  0001 C CNN
F 3 "" H 9000 4000 50  0001 C CNN
	1    9000 4000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR042
U 1 1 589ACFA9
P 4050 4300
F 0 "#PWR042" H 4050 4050 50  0001 C CNN
F 1 "GND" H 4050 4150 50  0000 C CNN
F 2 "" H 4050 4300 50  0000 C CNN
F 3 "" H 4050 4300 50  0000 C CNN
	1    4050 4300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR043
U 1 1 589ACFAA
P 9000 4250
F 0 "#PWR043" H 9000 4000 50  0001 C CNN
F 1 "GND" H 9000 4100 50  0000 C CNN
F 2 "" H 9000 4250 50  0000 C CNN
F 3 "" H 9000 4250 50  0000 C CNN
	1    9000 4250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR044
U 1 1 589ACFAB
P 5850 4600
F 0 "#PWR044" H 5850 4350 50  0001 C CNN
F 1 "GND" H 5850 4450 50  0000 C CNN
F 2 "" H 5850 4600 50  0000 C CNN
F 3 "" H 5850 4600 50  0000 C CNN
	1    5850 4600
	1    0    0    -1  
$EndComp
$Comp
L AAT1217-3.3 U701
U 1 1 589ACFAD
P 5850 3950
F 0 "U701" H 5400 4250 50  0000 C CNN
F 1 "AAT1217-1.2" H 6100 3650 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-23-6_Handsoldering" H 5850 3950 50  0001 C CNN
F 3 "DOCUMENTATION" H 5850 3950 50  0001 C CNN
F 4 "AAT1217ICA-1.2-T1" H 100 400 50  0001 C CNN "MPN"
F 5 "863-1495-1-ND" H 5850 3950 50  0001 C CNN "DigiKey"
	1    5850 3950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR045
U 1 1 589ACFC4
P 1650 5400
F 0 "#PWR045" H 1650 5150 50  0001 C CNN
F 1 "GND" H 1650 5250 50  0000 C CNN
F 2 "" H 1650 5400 50  0000 C CNN
F 3 "" H 1650 5400 50  0000 C CNN
	1    1650 5400
	1    0    0    -1  
$EndComp
$Comp
L L_Small L701
U 1 1 59D3C5CA
P 5150 3400
F 0 "L701" H 5180 3440 50  0000 L CNN
F 1 "4.7uH" H 5180 3360 50  0000 L CNN
F 2 "Inductors:Inductor_Taiyo-Yuden_MD-3030" H 5150 3400 50  0001 C CNN
F 3 "" H 5150 3400 50  0001 C CNN
F 4 "SRN3015TA-4R7M" H 5150 3400 60  0001 C CNN "MPN"
F 5 "SRN3015TA-4R7MCT-ND" H 5150 3400 60  0001 C CNN "DigiKey"
	1    5150 3400
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R704
U 1 1 59DEF5DE
P 7050 4000
F 0 "R704" H 7080 4020 50  0000 L CNN
F 1 "1.02M" H 7080 3960 50  0000 L CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 7050 4000 50  0001 C CNN
F 3 "" H 7050 4000 50  0001 C CNN
F 4 "RC0805FR-071M02L" H 7050 4000 60  0001 C CNN "MPN"
F 5 "311-1.02MCRCT-ND" H 7050 4000 60  0001 C CNN "DigiKey"
	1    7050 4000
	1    0    0    -1  
$EndComp
$Comp
L R_Small R705
U 1 1 59DEF739
P 7050 4350
F 0 "R705" H 7080 4370 50  0000 L CNN
F 1 "536K" H 7080 4310 50  0000 L CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 7050 4350 50  0001 C CNN
F 3 "" H 7050 4350 50  0001 C CNN
F 4 "RC0805FR-07536KL" H 7050 4350 60  0001 C CNN "MPN"
F 5 "311-536KCRCT-ND" H 7050 4350 60  0001 C CNN "DigiKey"
	1    7050 4350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR046
U 1 1 59DEF7BC
P 7050 4650
F 0 "#PWR046" H 7050 4400 50  0001 C CNN
F 1 "GND" H 7050 4500 50  0000 C CNN
F 2 "" H 7050 4650 50  0000 C CNN
F 3 "" H 7050 4650 50  0000 C CNN
	1    7050 4650
	1    0    0    -1  
$EndComp
Text Notes 7350 5350 0    60   ~ 0
Vout = 1.23 * ( 1 + (R1 / R2))\nVout   R1     R2\n3.3v  1.02M   604k\n3.57v 1.02M   536k <\n4.0v  1.02M   453k\n4.2v  1.02M   422k\n4.5v  1.02M   388k~\n5.0v  1.02M   332k
Text Notes 6850 4350 0    60   ~ 12
R1\n\n\nR2
Text Notes 600  4650 0    60   ~ 0
Two AAA in\nseries, so VBatt\nis 3v
Wire Wire Line
	5850 4500 5850 4600
Wire Wire Line
	4050 4100 4050 4300
Wire Wire Line
	4050 3800 4050 3900
Wire Wire Line
	5000 3800 5000 3400
Wire Wire Line
	5000 3400 5050 3400
Wire Wire Line
	5250 3400 5850 3400
Wire Wire Line
	9350 3800 9350 3700
Wire Wire Line
	1650 5250 1650 5400
Wire Wire Line
	9000 3900 9000 3800
Wire Wire Line
	9000 4100 9000 4250
Connection ~ 5000 3800
Connection ~ 9000 3800
Wire Wire Line
	7050 4100 7050 4250
Wire Wire Line
	7050 4150 6800 4150
Wire Wire Line
	6800 4150 6800 4100
Wire Wire Line
	6800 4100 6650 4100
Connection ~ 7050 4150
Wire Wire Line
	7050 4450 7050 4650
Wire Wire Line
	6650 3800 9350 3800
Wire Wire Line
	7050 3800 7050 3900
Connection ~ 7050 3800
$Comp
L GND #PWR047
U 1 1 5A10B1D8
P 5800 7350
F 0 "#PWR047" H 5800 7100 50  0001 C CNN
F 1 "GND" H 5800 7200 50  0000 C CNN
F 2 "" H 5800 7350 50  0000 C CNN
F 3 "" H 5800 7350 50  0000 C CNN
	1    5800 7350
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG048
U 1 1 5A10B43F
P 5800 7150
F 0 "#FLG048" H 5800 7225 50  0001 C CNN
F 1 "PWR_FLAG" H 5800 7300 50  0000 C CNN
F 2 "" H 5800 7150 50  0001 C CNN
F 3 "" H 5800 7150 50  0001 C CNN
	1    5800 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 7150 5800 7350
$Comp
L GND #PWR049
U 1 1 5A1511FD
P 7550 2300
F 0 "#PWR049" H 7550 2050 50  0001 C CNN
F 1 "GND" H 7550 2150 50  0000 C CNN
F 2 "" H 7550 2300 50  0000 C CNN
F 3 "" H 7550 2300 50  0000 C CNN
	1    7550 2300
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR050
U 1 1 5A854E40
P 9350 3700
F 0 "#PWR050" H 9350 3550 50  0001 C CNN
F 1 "+3V3" H 9350 3840 50  0000 C CNN
F 2 "" H 9350 3700 50  0001 C CNN
F 3 "" H 9350 3700 50  0001 C CNN
	1    9350 3700
	1    0    0    -1  
$EndComp
$Comp
L Keystone_2462 B701
U 1 1 5A853A70
P 1650 4550
F 0 "B701" V 1450 4150 50  0000 L CNN
F 1 "Keystone_2462" V 1850 4150 50  0000 L CNN
F 2 "Borgel Custom:Keystone_2462" H 1150 4025 50  0001 C CNN
F 3 "DOCUMENTATION" H 1300 4075 50  0001 C CNN
F 4 "36-2462-ND" H 1250 3950 50  0001 C CNN "DigiKey"
	1    1650 4550
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG051
U 1 1 5A868ACD
P 2000 3500
F 0 "#FLG051" H 2000 3575 50  0001 C CNN
F 1 "PWR_FLAG" H 2000 3650 50  0000 C CNN
F 2 "" H 2000 3500 50  0001 C CNN
F 3 "" H 2000 3500 50  0001 C CNN
	1    2000 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 3500 2000 3700
Wire Wire Line
	4600 4100 5050 4100
$Comp
L TEST_1P J704
U 1 1 5A9B1D07
P 8550 2250
F 0 "J704" H 8550 2520 50  0000 C CNN
F 1 "3v3" H 8550 2450 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 8750 2250 50  0001 C CNN
F 3 "" H 8750 2250 50  0001 C CNN
	1    8550 2250
	-1   0    0    1   
$EndComp
$Comp
L +3V3 #PWR053
U 1 1 5A9B1D4A
P 8700 2050
F 0 "#PWR053" H 8700 1900 50  0001 C CNN
F 1 "+3V3" H 8700 2190 50  0000 C CNN
F 2 "" H 8700 2050 50  0001 C CNN
F 3 "" H 8700 2050 50  0001 C CNN
	1    8700 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2250 8550 2200
Wire Wire Line
	8550 2200 8850 2200
Connection ~ 8700 2200
Wire Wire Line
	8850 2200 8850 2250
$Comp
L TEST_1P J705
U 1 1 5A9B1ED9
P 8700 2250
F 0 "J705" H 8700 2520 50  0000 C CNN
F 1 "3v3" H 8700 2450 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 8900 2250 50  0001 C CNN
F 3 "" H 8900 2250 50  0001 C CNN
	1    8700 2250
	-1   0    0    1   
$EndComp
$Comp
L TEST_1P J706
U 1 1 5A9B1F3A
P 8850 2250
F 0 "J706" H 8850 2520 50  0000 C CNN
F 1 "3v3" H 8850 2450 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 9050 2250 50  0001 C CNN
F 3 "" H 9050 2250 50  0001 C CNN
	1    8850 2250
	-1   0    0    1   
$EndComp
Wire Wire Line
	8700 2050 8700 2250
$Comp
L TEST_1P J703
U 1 1 5A9B22CF
P 7700 2050
F 0 "J703" H 7700 2320 50  0000 C CNN
F 1 "GND" H 7700 2250 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7900 2050 50  0001 C CNN
F 3 "" H 7900 2050 50  0001 C CNN
	1    7700 2050
	1    0    0    -1  
$EndComp
$Comp
L TEST_1P J702
U 1 1 5A9B239E
P 7550 2050
F 0 "J702" H 7550 2320 50  0000 C CNN
F 1 "GND" H 7550 2250 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7750 2050 50  0001 C CNN
F 3 "" H 7750 2050 50  0001 C CNN
	1    7550 2050
	1    0    0    -1  
$EndComp
$Comp
L TEST_1P J701
U 1 1 5A9B23DD
P 7400 2050
F 0 "J701" H 7400 2320 50  0000 C CNN
F 1 "GND" H 7400 2250 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7600 2050 50  0001 C CNN
F 3 "" H 7600 2050 50  0001 C CNN
	1    7400 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 2050 7550 2300
Wire Wire Line
	7700 2150 7700 2050
Wire Wire Line
	7400 2150 7700 2150
Connection ~ 7550 2150
Wire Wire Line
	7400 2050 7400 2150
$Comp
L TEST_1P J709
U 1 1 5A9F84FE
P 4200 3600
F 0 "J709" H 4200 3870 50  0000 C CNN
F 1 "VBatt" H 4200 3800 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 4400 3600 50  0001 C CNN
F 3 "" H 4400 3600 50  0001 C CNN
	1    4200 3600
	1    0    0    -1  
$EndComp
$Comp
L TEST_1P J708
U 1 1 5A9F866B
P 1200 2150
F 0 "J708" H 1200 2420 50  0000 C CNN
F 1 "GND" H 1200 2350 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 1400 2150 50  0001 C CNN
F 3 "" H 1400 2150 50  0001 C CNN
	1    1200 2150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR054
U 1 1 5A9F885C
P 1200 2300
F 0 "#PWR054" H 1200 2050 50  0001 C CNN
F 1 "GND" H 1200 2150 50  0000 C CNN
F 2 "" H 1200 2300 50  0000 C CNN
F 3 "" H 1200 2300 50  0000 C CNN
	1    1200 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 2150 1200 2300
$Comp
L SW_SPDT SW701
U 1 1 5AC5B4E0
P 2700 3800
F 0 "SW701" H 2700 3970 50  0000 C CNN
F 1 "JS102011SAQN" H 2700 3600 50  0000 C CNN
F 2 "Borgel Custom:CK-JS102011SAQN_sm-spdt-switch" H 2700 3800 50  0001 C CNN
F 3 "" H 2700 3800 50  0001 C CNN
F 4 "JS102011SAQN" H 2700 3800 60  0001 C CNN "MPN"
F 5 "401-1999-1-ND" H 2700 3800 60  0001 C CNN "DigiKey"
	1    2700 3800
	-1   0    0    1   
$EndComp
$Comp
L GS2 J710
U 1 1 5AC5BD84
P 2700 3300
F 0 "J710" H 2800 3450 50  0000 C CNN
F 1 "GS2" H 2800 3151 50  0000 C CNN
F 2 "Borgel Custom:SOLDER-JUMPER_1-WAY-SM" V 2774 3300 50  0001 C CNN
F 3 "" H 2700 3300 50  0001 C CNN
	1    2700 3300
	0    1    1    0   
$EndComp
$Comp
L +BATT #PWR055
U 1 1 5AE977BF
P 1650 3500
F 0 "#PWR055" H 1650 3350 50  0001 C CNN
F 1 "+BATT" H 1650 3640 50  0000 C CNN
F 2 "" H 1650 3500 50  0001 C CNN
F 3 "" H 1650 3500 50  0001 C CNN
	1    1650 3500
	1    0    0    -1  
$EndComp
Text Notes 3700 3050 0    60   ~ 0
Place J711 near prog header
$Comp
L TEST_1P J711
U 1 1 5AE9799C
P 4450 3600
F 0 "J711" H 4450 3870 50  0000 C CNN
F 1 "VBatt" H 4450 3800 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 4650 3600 50  0001 C CNN
F 3 "" H 4650 3600 50  0001 C CNN
	1    4450 3600
	1    0    0    -1  
$EndComp
Text Notes 8750 3400 0    60   ~ 0
Depending on trim of boost, this\nmay actually be 3.6v
$Comp
L R_Small R703
U 1 1 5AEAA8AA
P 4600 3950
F 0 "R703" H 4630 3970 50  0000 L CNN
F 1 "1.02M" H 4630 3910 50  0000 L CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 4600 3950 50  0001 C CNN
F 3 "" H 4600 3950 50  0001 C CNN
F 4 "RC0805FR-071M02L" H 4600 3950 60  0001 C CNN "MPN"
F 5 "311-1.02MCRCT-ND" H 4600 3950 60  0001 C CNN "DigiKey"
	1    4600 3950
	1    0    0    -1  
$EndComp
Connection ~ 4050 3800
Wire Wire Line
	4600 4100 4600 4050
Wire Wire Line
	4600 3800 4600 3850
Connection ~ 4600 3800
Wire Wire Line
	2900 3300 3200 3300
Wire Wire Line
	3200 3300 3200 3800
Connection ~ 3200 3800
Wire Wire Line
	1650 3700 2500 3700
Wire Wire Line
	1650 3500 1650 3900
Connection ~ 2000 3700
Connection ~ 1650 3700
Wire Wire Line
	2500 3300 2250 3300
Wire Wire Line
	2250 3300 2250 3700
Connection ~ 2250 3700
NoConn ~ 2500 3900
Wire Wire Line
	4450 3800 4450 3600
Connection ~ 4450 3800
Wire Wire Line
	4200 3800 4200 3600
Connection ~ 4200 3800
Wire Wire Line
	2900 3800 5050 3800
$Comp
L PWR_FLAG #FLG056
U 1 1 5AEADA5D
P 3750 3550
F 0 "#FLG056" H 3750 3625 50  0001 C CNN
F 1 "PWR_FLAG" H 3750 3700 50  0000 C CNN
F 2 "" H 3750 3550 50  0001 C CNN
F 3 "" H 3750 3550 50  0001 C CNN
	1    3750 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 3550 3750 3800
Connection ~ 3750 3800
$EndSCHEMATC