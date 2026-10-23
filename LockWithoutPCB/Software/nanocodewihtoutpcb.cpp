#include <Wire.h>
#include <Adafruit_PN532.h>
#include <SoftwareSerial.h>

// --- PIN DEFINITIONS ---
// PN532 I2C Pins are hardware A4 (SDA) and A5 (SCL)
#define PN532_IRQ   (2)
#define PN532_RESET (3)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// RGB LED (Common Cathode)
#define LED_R 9
#define LED_G 10
#define LED_B 11

// UI Button
#define BTN_PIN 2 // Spliced with Cat5e Wake wire

// Software Serial to ESP32 (Leaves hardware USB serial free for debugging)
SoftwareSerial espSerial(4, 5); // RX, TX

// Replace this with your actual NFC Tag UID
uint8_t authorizedUID[] = { 0x04, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12 };
uint8_t uidLength = 7; 

bool isLocked = true;
unsigned long lastDebounceTime = 0;
bool lastButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  espSerial.begin(9600);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // Halt
  }
  nfc.SAMConfig();
  
  setLED(255, 0, 0); // Start Red (Locked)
  Serial.println("Nano UI Ready.");
}

void loop() {
  checkButton();
  checkNFC();
  listenToESP32();
}

void checkButton() {
  // Push button is ONLY used to lock the bike
  bool reading = digitalRead(BTN_PIN);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > 50) {
    if (reading == LOW && !isLocked) {
      Serial.println("Button Pressed: Sending Lock Command...");
      espSerial.print('L');
      setLED(255, 255, 0); // Yellow (Processing)
      delay(1000); // Prevent spamming
    }
  }
  lastButtonState = reading;
}

void checkNFC() {
  // NFC is ONLY used to unlock the bike
  if (!isLocked) return; 

  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLen;

  // Non-blocking read (timeout 50ms)
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 50)) {
    Serial.println("Card Detected!");
    
    // Check if UID matches
    bool match = true;
    for (uint8_t i = 0; i < uidLen; i++) {
      if (uid[i] != authorizedUID[i]) match = false;
    }

    if (match) {
      Serial.println("Authorized! Sending Unlock Command...");
      espSerial.print('U');
      setLED(0, 0, 255); // Blue (Processing)
      delay(2000); // Prevent multi-scans
    } else {
      Serial.println("Unauthorized Card.");
      blinkLED(255, 0, 0, 3); // Fast flash red
    }
  }
}

void listenToESP32() {
  if (espSerial.available()) {
    char status = espSerial.read();
    
    if (status == 'M') {
      // Motion Lockout Error
      Serial.println("ESP32: Motion Detected! Cannot lock.");
      blinkLED(255, 100, 0, 5); // Fast flash Orange
      setLED(0, 255, 0); // Return to Unlocked Green
    } 
    else if (status == 'K') {
      // Lock Confirmed
      isLocked = true;
      setLED(255, 0, 0); // Solid Red
    } 
    else if (status == 'O') {
      // Unlock Confirmed
      isLocked = false;
      setLED(0, 255, 0); // Solid Green
    }
    else if (status == 'A') {
      // Alarm Triggered
      while(true) { // Infinite alarm loop until reset
        setLED(255, 0, 0); delay(100);
        setLED(0, 0, 255); delay(100);
      }
    }
  }
}

void setLED(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}

void blinkLED(int r, int g, int b, int times) {
  for (int i = 0; i < times; i++) {
    setLED(r, g, b);
    delay(150);
    setLED(0, 0, 0);
    delay(150);
  }
}