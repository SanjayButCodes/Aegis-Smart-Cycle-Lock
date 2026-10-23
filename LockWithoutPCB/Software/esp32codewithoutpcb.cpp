#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// --- PIN DEFINITIONS ---
// Motor Driver (TB6612FNG)
#define AIN1 25
#define AIN2 26
#define PWMA 27

// Limit Switches
#define SW_UNLOCK 32
#define SW_LOCK 33

// Security & Alarm
#define SIREN_PIN 4
#define DEADMAN_PIN 14 // Cat5e loop return (with 10k pulldown)
#define WAKE_BTN 35    // Signal from Nano Button

// Serial Communication to Nano
#define RX_NANO 16
#define TX_NANO 17

// Motion threshold (m/s^2) for lockout
#define MOTION_THRESHOLD 2.0 

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX_NANO, TX_NANO); // Comm with Nano

  // Init Pins
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(SIREN_PIN, OUTPUT);
  
  pinMode(SW_UNLOCK, INPUT_PULLUP);
  pinMode(SW_LOCK, INPUT_PULLUP);
  pinMode(DEADMAN_PIN, INPUT); 
  pinMode(WAKE_BTN, INPUT);

  stopMotor();
  digitalWrite(SIREN_PIN, LOW);

  // Init MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
  } else {
    mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
    mpu.setMotionDetectionThreshold(2);
    mpu.setMotionDetectionDuration(20);
    mpu.setInterruptPinLatch(true);
    mpu.setInterruptPinPolarity(true);
    mpu.setMotionInterrupt(true);
  }

  checkWakeupReason();
}

void loop() {
  // 1. Check Dead-Man Loop
  if (digitalRead(DEADMAN_PIN) == LOW) {
    triggerAlarm("CABLE CUT");
  }

  // 2. Listen for UI Commands
  if (Serial2.available()) {
    char cmd = Serial2.read();
    if (cmd == 'U') unlockBike();
    if (cmd == 'L') lockBike();
    if (cmd == 'S') goToSleep();
  }

  delay(50);
}

void lockBike() {
  // MOTION LOCKOUT CHECK: Do not lock if bike is moving
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  float motion = abs(a.acceleration.x) + abs(a.acceleration.y);
  if (motion > MOTION_THRESHOLD) {
    Serial.println("Lock Aborted: Bike is in motion!");
    Serial2.print('M'); // Send Motion Error to Nano
    return;
  }

  Serial.println("Locking...");
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 255);

  unsigned long startTime = millis();
  while (digitalRead(SW_LOCK) == HIGH) {
    if (millis() - startTime > 4000) break; // 4-second timeout
  }
  
  stopMotor();
  Serial2.print('K'); // Send 'K' (LocKed) status to Nano
  delay(1000);
  goToSleep();
}

void unlockBike() {
  Serial.println("Unlocking...");
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 255);

  unsigned long startTime = millis();
  while (digitalRead(SW_UNLOCK) == HIGH) {
    if (millis() - startTime > 4000) break; 
  }
  
  stopMotor();
  Serial2.print('O'); // Send 'O' (Open) status to Nano
}

void stopMotor() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);
}

void triggerAlarm(String reason) {
  Serial.println("ALARM TRIGGERED: " + reason);
  Serial2.print('A'); // Tell Nano to flash lights
  
  // Pulse Siren for 30 seconds
  for (int i = 0; i < 30; i++) {
    digitalWrite(SIREN_PIN, HIGH);
    delay(500);
    digitalWrite(SIREN_PIN, LOW);
    delay(500);
  }
}

void goToSleep() {
  Serial.println("Entering Deep Sleep...");
  // Wake if button pressed (Nano) OR cable cut (Deadman drops to LOW)
  esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKE_BTN, 1);
  esp_sleep_enable_ext1_wakeup(1ULL << DEADMAN_PIN, ESP_EXT1_WAKEUP_ALL_LOW);
  
  delay(500);
  esp_deep_sleep_start();
}

void checkWakeupReason() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    triggerAlarm("CABLE CUT WHILE ASLEEP");
  } else if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Woke up from Nano Button");
  }
}