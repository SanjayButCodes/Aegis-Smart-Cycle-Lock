# Aegis Smart Cycle Lock: Component Master Guide

## 1. Power Delivery & Management
The power architecture of the Aegis Smart Cycle Lock is engineered to provide high efficiency, system stability under inductive motor loads, and reliable fault protection.

### 18650 Li-ion Battery Cell
* **Role:** Primary energy source for the entire system.
* **Specifications:** 3.7V Nominal Voltage, 4.2V Peak Charge, minimum 2500mAh capacity (high-drain cell recommended, >= 5A continuous discharge).
* **Implementation:** Standard single-cell configuration housed in a high-retention holder within the main rear vault housing.

### TP4056 Type-C Charging Module (Protection IC Variant)
* **Role:** Safe USB-C charging management and baseline battery protection.
* **Critical Requirement:** The module **must** feature the DW01A protection IC and 8205A dual MOSFET switch network (identifiable by the 6-pin and 8-pin ICs near the output pads).
* **Protection Features:**
  * **Over-charge Protection:** Cuts off charging at 4.28V.
  * **Over-discharge Protection:** Disconnects the load when battery voltage drops below 2.4V to prevent cell degradation.
  * **Over-current Protection:** Limits output current spikes to 3A.

### MT3608 Step-Up (Boost) Converters
The design utilizes **two independent MT3608 converter modules** tied to the battery output to eliminate line noise interference and voltage sag during motor engagement.
* **Module A (5V Logic Rail):**
  * **Tuning Target:** 5.0V DC.
  * **Purpose:** Supplies power to the ESP32 (via Vin), Arduino Nano (5V), PN532 NFC reader, status LEDs, and sensors.
* **Module B (12V Actuation Rail):**
  * **Tuning Target:** 12.0V DC.
  * **Purpose:** Dedicated high-voltage rail for driving the JGY370 worm gear motor and the high-decibel piezo siren.

### Stabilization Capacitor
* **Component:** 1000µF, 16V Aluminum Electrolytic Capacitor.
* **Placement:** Placed parallel to the 12V output rail, directly across the power terminals of the motor driver.
* **Function:** Serves as a local energy reservoir to absorb high inrush currents (up to 1.2A stall current) during motor startup, preventing voltage dips on the 5V logic rail that could cause brownout resets on the microcontrollers.

---

## 2. The Vault (Rear Hub)
The Vault serves as the central control and physical lock housing mounted above the rear wheel assembly.

| Component | Operating Voltage | Interface Type | Primary Function |
| :--- | :--- | :--- | :--- |
| **ESP32 Dev Board** | 5.0V (Vin) / 3.3V Logic | UART / SPI / I2C / GPIO | System Master, Wireless Telemetry, State Logic |
| **TB6612FNG Driver** | 2.7V–5.5V (Logic), 12V (Motor) | PWM / Digital GPIO | Dual H-Bridge Motor Control |
| **JGY370 Gear Motor** | 12V DC | Mechanical Shaft | Physical Bolt Actuation |
| **MPU6050 Module** | 3.3V | I2C (0x68) | 6-DOF Tamper & Anti-Theft Sensing |
| **Piezo Siren** | 12V DC | Low-Side N-Ch Switch | 110dB+ Acoustic Alarm |

### ESP32 Development Board
Acts as the central system master. Handles lock/unlock logic, sensor processing, motor state machine execution, and telemetry interfaces. Communicates with the Handlebar Hub via serial data lines over the interconnect cable.

### TB6612FNG Dual Motor Driver
* **Advantage:** Replaces inefficient L298N drivers by using MOSFET H-bridges, offering higher efficiency and lower thermal output.
* **Control Pins:**
  * **AIN1 / AIN2:** Directional control pins connected to ESP32 GPIOs.
  * **PWMA:** Speed control tied to an ESP32 PWM-capable pin (set to 100% duty cycle for maximum lock torque).
  * **STBY:** Hard-wired to 3.3V logic high to keep the driver active during operational states.

### JGY370 Worm Gear DC Motor
* **Mechanism:** High-torque 12V DC motor integrated with a right-angle worm gear reducer (typically 10–30 RPM depending on gear ratio).
* **Physical Anti-Theft Locking:** The fundamental geometry of a worm gear provides an inherent **mechanical self-locking mechanism**. The lead screw on the motor shaft can easily drive the worm wheel, but the worm wheel **cannot back-drive** the lead screw. Any physical force applied to manual pull or force the locking pin open is absorbed by the mechanical housing, preventing unauthorized back-driving without drawing electrical power.

### Roller Lever Microswitches
* **Quantity:** 2 Switches (Positioned at absolute EXTENDED and RETRACTED mechanical limits).
* **Wiring:** Configured in Normally Open (NO) with internal pull-up resistors activated on the ESP32 logic inputs.
* **Function:** Provides physical hardware interrupts to instantly stop motor drive upon complete lock engagement or disengagement, preventing motor stalling and mechanical wear.

### MPU6050 Motion Sensor (Accelerometer + Gyroscope)
* **Function:** Monitors the physical state of the cycle frame to detect tampering or theft attempts.
* **Threshold Logic:**
  * Monitors raw 3-axis accelerometer values to calculate total vector acceleration:
    $$ \text{Magnitude} = \sqrt{A_x^2 + A_y^2 + A_z^2} $$
  * **2.0G Motion Lockout Threshold:** A threshold of $\ge 2.0\text{G}$ is programmed into the ESP32 firmware. Sustained acceleration or sudden impact exceeding this value while the system is in the LOCKED state triggers the anti-theft state machine (activating the siren and flashing alert indicators).

### 12V Piezo Alarm & Driving Circuit
* **Siren Specs:** High-decibel (110dB+ @ 12V) continuous tone siren module.
* **Driver Circuit:** Driven via an **IRLZ44N Logic-Level N-Channel MOSFET** arranged in a low-side switching configuration:
  * **Gate:** Driven directly by an ESP32 GPIO pin through a 220Ω current-limiting resistor. A 10kΩ pull-down resistor from Gate to GND ensures the siren remains completely silent during system boot/reset phases.
  * **Drain:** Connected to the negative terminal of the 12V Piezo Siren.
  * **Source:** Tied to Common System Ground (GND).
  * **Flyback Protection:** A 1N4007 diode is placed in reverse-parallel across the siren terminals to dissipate inductive voltage spikes.

---

## 3. The UI (Handlebar Hub)
Located on the handlebars, this unit acts as the user interface and authentication node, communicating back to the Vault master unit.

### Arduino Nano (ATmega328P)
* **Role:** Local Hub Controller. Offloads NFC polling, LED pattern generation, and button debounce routines from the main ESP32 processor. Communicates events via a high-speed UART serial connection.

### PN532 NFC/RFID Reader Module
* **Protocol & Wiring:** Configured to **I2C mode** via the onboard DIP switches (Channel 1: ON, Channel 2: OFF).
  * **SDA** connected to Arduino Nano pin A4.
  * **SCL** connected to Arduino Nano pin A5.
  * **IRQ** connected to Nano pin D2 for low-power hardware interrupt handling when an RFID tag or smartphone approaches.
* **Function:** Reads 13.56MHz ISO/IEC 14443A keyfobs, cards, or mobile device payloads for contact-free unlocking.

### Common Cathode RGB LED
* **Wiring:** Common cathode connected to GND. Red, Green, and Blue anodes wired through 220Ω (Red) and 100Ω (Green/Blue) current-limiting resistors to Arduino Nano PWM pins (D3, D5, D6).
* **Visual Status Feedback Matrix:**
  * **Solid Blue:** System ready / Standby (Unlocked).
  * **Flashing Amber:** Authentication in progress / Motor actuating.
  * **Solid Green:** Unlocked successfully (Active for 5 seconds).
  * **Pulsing Red:** Locked state active.
  * **Rapid Strobing Red:** Tamper alarm triggered / Threat detected.

### Waterproof Tactile Push Button
* **Role:** Manual user input button (Lock command trigger / Wake button).
* **Specs:** Panel-mount IP67 waterproof momentary switch.
* **Wiring:** Connected between Nano GPIO pin D7 and GND, using internal `INPUT_PULLUP` mode and debounced via software logic (20ms lockout filter).

---

## 4. Hardware & Interconnects

### Inter-Hub Cabling: Cat5e Ethernet Cable
To connect the Handlebar Hub (UI) to the Rear Vault Hub safely across the bicycle frame, a standard **Cat5e shielded twisted pair (STP) cable** is deployed. The 8 conductors are mapped as follows:

| Pin | Pair Color | Signal Name | Description |
| :--- | :--- | :--- | :--- |
| **1** | Orange/White | `VCC_5V` | Main 5V Logic Power Bus |
| **2** | Orange | `GND` | Common Ground Connection |
| **3** | Green/White | `TX_DATA` | UI Transmit -> Vault Receive |
| **4** | Blue | `RX_DATA` | UI Receive <- Vault Transmit |
| **5** | Blue/White | `WAKE_INT` | Wake-on-Button Interrupt Line |
| **6** | Green | `DEADMAN_SEND` | Security Loop Outbound |
| **7** | Brown/White | `DEADMAN_RET` | Security Loop Inbound |
| **8** | Brown | `SHIELD_GND` | Frame / Cable Shield Drain |

### Cable Interface: RJ45 Breakout Boards
* **Implementation:** Screw-terminal RJ45 breakout modules are permanently mounted inside both the Handlebar and Rear Vault enclosures.
* **Advantage:** Enables modular assembly, simple wire routing along the bike frame, and easy maintenance or replacement of individual sub-assemblies.
* **Security Loop ("Dead-Man Line"):** Pins 6 and 7 continuously route a low-current sense voltage loop through the length of the cable. If an attacker cuts or unplugs the Cat5e cable, the ESP32 instantly detects the loss of the loop circuit and trips the 12V siren locally in the Vault node via battery power.

### Mechanical Frame & Housing
* **Base Frame:** Utilizes a heavily modified commercial **"Dutch-Style" frame/ring lock chassis**.
* **Modification Matrix:**
  * **Shackle Pin:** Original manual slide lever removed; replaced with an internal hardened steel locking rod directly linked to the JGY370 worm gear output linkage.
  * **Enclosure Extension:** 3D-printed PETG high-impact weather-resistant housing bolted directly to the metallic Dutch lock base frame to securely encapsulate the electronics, motor, battery power management system, and sensors.