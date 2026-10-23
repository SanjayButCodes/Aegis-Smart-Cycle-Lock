EESchema Schematic File Version 4
LIBS:power
LIBS:Connector_Generic
LIBS:MCU_Module
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
Sheet 1 1
Title "Handlebar Hub rev B — 60 x 40 mm, 5 V UI Board"
Comment1 "KiCad 8 imports this legacy schematic; native netlist is supplied separately."
Comment2 "J1.5 and J1.6 form the external continuity tamper loop; rear hub must sense it."
Comment3 "Rev B places Nano lengthwise on 60 mm board; see active rev-B PCB."
$EndDescr
Text Notes 700 650 0 100 ~ 20
HANDLEBAR HUB — 5 V ONLY
$Comp
L MCU_Module:Arduino_Nano_v3.x A1
U 1 1 1
P 5200 3500
F 0 "A1" H 5200 2411 50  0000 C CNN
F 1 "Arduino_Nano" H 5200 2320 50 0000 C CNN
F 2 "Module:Arduino_Nano" H 5350 2550 50 0001 L CNN
	1    5200 3500
	1    0    0    -1
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J1
U 1 1 2
P 1900 2100
F 0 "J1" H 1818 2517 50 0000 C CNN
F 1 "BRIDGE_6P (5.08 mm)" H 1818 2426 50 0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x06_P2.50mm_Vertical" H 1900 2100 50 0001 C CNN
	1    1900 2100
	-1 0 0 -1
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J2
U 1 1 3
P 8500 2600
F 0 "J2" H 8580 2592 50 0000 L CNN
F 1 "PN532 I2C" H 8580 2501 50 0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 8500 2600 50 0001 C CNN
	1    8500 2600
	1 0 0 -1
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 4
P 8500 4000
F 0 "J3" H 8580 3992 50 0000 L CNN
F 1 "BUTTON" H 8580 3901 50 0000 L CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 8500 4000 50 0001 C CNN
	1    8500 4000
	1 0 0 -1
$EndComp
$Comp
L Device:LED_RGB D1
U 1 1 5
P 8500 5350
F 0 "D1" H 8500 5847 50 0000 C CNN
F 1 "RGB common cathode" H 8500 5756 50 0000 C CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB_Staggered_Pins" H 8500 5300 50 0001 C CNN
	1    8500 5350
	1 0 0 -1
$EndComp
$Comp
L Device:R R1
U 1 1 6
P 7200 5200
F 0 "R1" V 6993 5200 50 0000 C CNN
F 1 "220R 0805" V 7084 5200 50 0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7130 5200 50 0001 C CNN
	1    7200 5200
	0 1 1 0
$EndComp
$Comp
L Device:R R2
U 1 1 7
P 7200 5400
F 0 "R2" V 7100 5400 50 0000 C CNN
F 1 "220R 0805" V 7300 5400 50 0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7130 5400 50 0001 C CNN
	1    7200 5400
	0 1 1 0
$EndComp
$Comp
L Device:R R3
U 1 1 8
P 7200 5600
F 0 "R3" V 7100 5600 50 0000 C CNN
F 1 "220R 0805" V 7300 5600 50 0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7130 5600 50 0001 C CNN
	1    7200 5600
	0 1 1 0
$EndComp
Text Label 2200 2100 0 50 ~ 0
+5V
Text Label 2200 2200 0 50 ~ 0
GND
Text Label 2200 2300 0 50 ~ 0
UART_TX
Text Label 2200 2400 0 50 ~ 0
UART_RX
Text Label 2200 2500 0 50 ~ 0
TAMPER_LOOP
Text Label 2200 2600 0 50 ~ 0
TAMPER_LOOP
Text Notes 1500 3000 0 50 ~ 0
Pin mapping: J1.1 +5V, .2 GND, .3 Nano D1/TX, .4 Nano D0/RX, .5/.6 tamper loop.
Text Notes 6500 6250 0 50 ~ 0
Net mapping: D2 button (internal pull-up); D3/R1 red; D5/R2 green; D6/R3 blue; A4 SDA; A5 SCL.
$EndSCHEMATC
