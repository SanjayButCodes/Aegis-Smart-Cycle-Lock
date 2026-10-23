#include <Wire.h>
#include <Adafruit_PN532.h>

// Hardware Pin Definitions based on PCB Rev B
#define LOCK_BUTTON_PIN 2   // Mapped to Nano D2
#define LED_R_PIN 3         // Mapped to Nano D3
#define LED_G_PIN 5         // Mapped to Nano D5
#define LED_B_PIN 6         // Mapped to Nano D6

// PN532 I2C Setup (using hardware I2C pins A4/A5)
#define PN532_IRQ   (4)     // Abstracted IRQ
#define PN532_RESET (7)     // Abstracted Reset
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup() {
  // Initialize UART bridge to ESP32 (Hardware Serial D0/D1)
  Serial.begin(9600); 
  
  pinMode(LOCK_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);

  nfc.begin();
  if (!nfc.getFirmwareVersion()) {
    while (1); // Halt if PN532 is missing
  }
  nfc.SAMConfig(); // Configure PN532 to read RFID tags
  setLED(0, 255, 0); // Boot to Green (Unlocked)
}

void loop() {
  // 1. Check for physical Lock Button press
  if (digitalRead(LOCK_BUTTON_PIN) == LOW) {
    Serial.println("CMD:LOCK");
    setLED(255, 0, 0); // Red
    delay(1000); // Software debounce
  }

  // 2. Poll NFC Reader (with 50ms timeout to keep loop non-blocking)
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
  
  if (success) {
    Serial.print("CMD:UNLOCK:");
    for (uint8_t i = 0; i < uidLength; i++) {
      if (uid[i] <= 0x0F) Serial.print("0"); // Pad with leading zero
      Serial.print(uid[i], HEX);
    }
    Serial.println();
    setLED(0, 255, 0); // Green
    delay(1000); // Prevent spamming payload
  }
}

void setLED(int r, int g, int b) {
  analogWrite(LED_R_PIN, r);
  analogWrite(LED_G_PIN, g);
  analogWrite(LED_B_PIN, b);
}