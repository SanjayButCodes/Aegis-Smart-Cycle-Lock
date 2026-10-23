# Ageis Smart Cycle Lock
Aegis is a heavy-duty, open-source, distributed smart bicycle lock. Unlike standard commercial locks, Aegis uses a split-node architecture to physically separate the user interface from the vulnerable locking mechanism, making it highly resistant to tampering and brute-force attacks.

🏗️ System Architecture

The system consists of two custom-designed nodes connected by a weather-sealed Cat5e bridge cable:

The Handlebar Hub (User Interface): A compact, IP65-rated pod mounted to the handlebars. It contains an Arduino Nano, an NFC card reader, an RGB status LED, and a locking button. It operates purely on 5V logic.

The Armored Rear Hub (The Brain & Brawn): A heavy-duty horseshoe frame lock mounted over the rear tire. It houses the ESP32 brain, a 12V worm-gear motor, a 3S battery pack, a 110dB siren, and an IMU accelerometer for theft detection.

The "Dead-Man Loop" Tamper Trace

A continuous 3.3V continuity loop originates at the Rear Hub, travels up the Cat5e cable, and traces the exact outer physical perimeter of the Handlebar Hub PCB. If a thief cuts the communication cable or attempts to crack open the Handlebar enclosure, the circuit breaks and the Rear Hub instantly triggers the 110dB siren.

⚡ Hardware Specifications

Armored Rear Hub (Main PCB)

Microcontroller: ESP32-WROOM-32U (Requires external antenna)

Power Supply: 3S (11.1V-12.6V) 18650 Battery Pack

Charging: IP2326 3S Type-C BMS charging IC

Logic Power: MP2315 Buck Converter (Steps 12.6V down to clean 3.3V)

Motor Driver: DRV8871 H-Bridge (Handling up to 2A stall current)

Actuator: JGY370 12V Worm Gear Motor

Sensors: MPU6050 Accelerometer (Wired to ESP32 ext0 wake interrupt)

Audio: 5V Active Piezo Siren

Handlebar Hub (UI PCB)

Microcontroller: Arduino Nano (DIP-30)

Authentication: PN532 NFC/RFID Module (I2C)

User Input: 12mm Waterproof Momentary Push Button

Feedback: Common Cathode RGB LED

Communication: 9600-baud UART over Cat5e Bridge

🛠️ Mechanical & CAD Design

The physical enclosures are designed to be FDM 3D-printed in highly impact-resistant materials like PETG, ABS, or Polycarbonate.

The Deadbolt: The locking mechanism uses a 12mm hardened steel rod acting as a deadbolt.

Rack and Pinion: The JGY370 motor drives a Module 1.5 pinion gear, converting rotational torque into linear motion to slide the deadbolt across the rear tire spokes.

Limit Switches: Two KW12-3 micro limit switches are installed at the ends of the deadbolt track to cut motor power precisely when fully locked or fully unlocked, preventing gear stripping.

Weatherproofing: The enclosures utilize overlapping lap joints designed to hold TPU gaskets or silicone bead seals to protect against rain and tire splash.

💻 Firmware & Software

The codebase is split into two environments:

/firmware/handlebar_nano/: Runs a non-blocking loop polling the PN532 for authorized MIFARE tags. Upon reading a tag or button press, it transmits a clean UART string (e.g., CMD:UNLOCK:UID) down to the rear hub.

/firmware/rear_esp32/: The central state machine. It manages the DRV8871 motor phases, parses UART commands, and handles extreme power conservation.

Deep Sleep Architecture

To allow the lock to run for months on a single charge, the ESP32 enters deep sleep immediately upon locking. It consumes only ~10µA until the MPU6050 accelerometer detects physical tampering, which triggers a hardware interrupt on GPIO 25, waking the ESP32 instantly to sound the alarm.

⚠️ High-Current Safety Warning

This project drives a high-torque 12V motor capable of drawing up to 2 Amps when stalled against a jammed lock.

PCB Routing: The VBAT and MOTOR nets on the Rear Hub PCB must be routed at a minimum of 4.0mm trace width.

Battery: Only use protected 3S battery packs with an integrated BMS. Never run standard 1S cells in series without balancing protection.

🚀 Getting Started

(Instructions for ordering PCBs, flashing code, and assembling the CAD files will be added here as the final manufacturing files are released.)
