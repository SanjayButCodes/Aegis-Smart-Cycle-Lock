#include <Arduino.h>

// State Machine Definitions
enum LockState { UNLOCKED, LOCKED, TAMPERED };
LockState currentState = UNLOCKED;

// Hardware UART Bridge (Mapped to PCB layout)
#define RXD2 5  // ESP32 GPIO5 (Through 5V-to-3V3 Divider)
#define TXD2 4  // ESP32 GPIO4
HardwareSerial NanoBridge(1);

// Hardware Pin Definitions
#define IMU_INT_PIN 25      // MPU6050 Interrupt Wakeup
#define MOTOR_PH 26         // DRV8871 Phase (Direction)
#define MOTOR_EN 27         // DRV8871 Enable (PWM)
#define BUZZ_EN 32          // Siren Transistor Base

// Authorized UID (Hardcoded for prototype; later fetch via BLE)
const String MASTER_KEY_UID = "0482B61A9F4080"; 

void setup() {
  Serial.begin(115200); // USB Debug
  NanoBridge.begin(9600, SERIAL_8N1, RXD2, TXD2);

  pinMode(MOTOR_PH, OUTPUT);
  pinMode(MOTOR_EN, OUTPUT);
  pinMode(BUZZ_EN, OUTPUT);

  // Configure Deep Sleep Wakeup Source (Wake on HIGH from MPU6050)
  esp_sleep_enable_ext0_wakeup((gpio_num_t)IMU_INT_PIN, 1);
  
  checkWakeupReason();
}

void loop() {
  // Listen for payload from the Handlebar Hub
  if (NanoBridge.available()) {
    String payload = NanoBridge.readStringUntil('\n');
    payload.trim();
    handleNanoCommand(payload);
  }

  // Handle Tamper State
  if (currentState == TAMPERED) {
    digitalWrite(BUZZ_EN, HIGH); // Sound 110dB Alarm
    // Add BLE broadcast or Campus Wi-Fi alert trigger here
  }
}

void handleNanoCommand(String cmd) {
  if (cmd == "CMD:LOCK" && currentState == UNLOCKED) {
    currentState = LOCKED;
    engageDeadbolt();
    
    // Shut down logic and sleep to conserve 3S battery
    Serial.println("System Locked. Entering Deep Sleep...");
    esp_deep_sleep_start();
  } 
  else if (cmd.startsWith("CMD:UNLOCK:")) {
    String tagUID = cmd.substring(11);
    tagUID.toUpperCase();
    
    if (tagUID == MASTER_KEY_UID) {
      currentState = UNLOCKED;
      digitalWrite(BUZZ_EN, LOW); // Silence alarm if active
      disengageDeadbolt();
    } else {
      Serial.println("Auth Failed: Unknown Tag");
    }
  }
}

void checkWakeupReason() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    // Woken up by physical vibration while locked
    currentState = TAMPERED;
  }
}

// --- Abstracted Hardware Functions ---
void engageDeadbolt() {
  // TODO: Implement DRV8871 forward logic until locked limit switch is hit
  digitalWrite(MOTOR_PH, HIGH);
  analogWrite(MOTOR_EN, 255);
  delay(1000); // Placeholder timing
  analogWrite(MOTOR_EN, 0);
}

void disengageDeadbolt() {
  // TODO: Implement DRV8871 reverse logic until unlocked limit switch is hit
  digitalWrite(MOTOR_PH, LOW);
  analogWrite(MOTOR_EN, 255);
  delay(1000); // Placeholder timing
  analogWrite(MOTOR_EN, 0);
}