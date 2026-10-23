# Named netlist and routing guide

## Power

| Net | Connections | PCB rule |
|---|---|---|
| USB_VBUS | J1 VBUS, input ESD/fuse, IP2326 VIN | Keep at USB connector; do not connect to VBAT |
| VBAT | protected 3S BMS output, U2 VM, U3 VIN, 5 V buck VIN | 4.0 mm outer-layer route/pour for 3 A validation |
| +3V3 | MP2315 output, U1, J5 MPU6050, dividers | 1.0 mm main branch; 0.30 mm signals |
| +5V | separate buck output, BZ1 via Q1, J4 pin 1 via eFuse | 1.0 mm; label eFuse current limit |
| GND | all supply returns, USB/bridge ESD, BMS output return | solid top/bottom pours with stitching vias |

## Logic

| ESP32 pin | Named net | Destination |
|---|---|---|
| GPIO21 | I2C_SDA | J5 MPU6050 SDA, 4.7 k pull-up to +3V3 |
| GPIO22 | I2C_SCL | J5 MPU6050 SCL, 4.7 k pull-up to +3V3 |
| GPIO25 | IMU_INT | J5 MPU6050 INT, 100 ohm series and 100 k pulldown |
| GPIO26 | MOTOR_PH | U2 PH input |
| GPIO27 | MOTOR_EN | U2 EN/PWM, 10 k pull-down |
| GPIO32 | BUZZ_EN | Q1 logic-NMOS gate, 100 ohm series / 100 k pulldown |
| GPIO5 | BRIDGE_RX_3V3 | J4 pin 3 through 20 k / 10 k divider from 5 V UART |
| GPIO4 | BRIDGE_TX_3V3 | J4 pin 4 through 1 k series resistor |
| GPIO33 | DEADMAN_IN | J4 pin 6, 100 k pulldown |

## Connector map

| Connector | Pin map |
|---|---|
| J2 Battery | 1 VBAT, 2 GND; connect only to the BMS protected discharge output |
| J3 Motor | 1 MOTOR_A, 2 MOTOR_B |
| J4 Bridge | 1 protected +5V, 2 GND, 3 remote 5 V TX to divider, 4 ESP32 TX, 5 DEADMAN_OUT, 6 DEADMAN_IN |
| J5 MPU6050 | +3V3, GND, I2C_SDA, I2C_SCL, IMU_INT; use the module's printed pin order |

## Release gates

1. Bind U2 and U4 to the exact manufacturer pin maps before assigning pad nets or producing Gerbers.
2. Complete the IP2326 3S-mode setting, current/voltage programming resistors, inductor, input/output capacitors, NTC, CC1/CC2 5.1 k resistors, TVS and USB ESD network from the chosen data sheet.
3. Add the dedicated 5 V buck and its eFuse; the active buzzer cannot run from the +3V3 rail.
4. Set the final motor current limit from measured JGY370 locked-rotor current; then rerun DRC and thermal testing.
5. Place the external 2.4 GHz antenna outside the metal hub; the WROOM-32U itself has no PCB antenna.
