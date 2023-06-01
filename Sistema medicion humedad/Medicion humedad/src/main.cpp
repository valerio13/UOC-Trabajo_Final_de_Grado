#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <ESP32Servo.h>
#include <ble.h>
#include <config.h>
#include <humidity.h>
#include <sstream>
#include <string>
const int servoPin = 18;

bool calibratingMin = false;
bool calibratingMax = false;

AppState currentAppState = RUNNING;

Servo servo;

// variable for storing the potentiometer value
int pos = 0;

BLEServer *server;
BLECharacteristic *pHumidityCharacteristic;

const char startCalibrationChar = '1';
void executeRunning();

void setup() {
  Serial.begin(921600);

  servo.attach(servoPin, 500, 2400);
  humiditySetup();
  bleSetup();

  delay(1000);
}

void loop() {
  if (currentAppState == RUNNING) {
    executeRunning();
  }

  bleLoop();

  delay(500);
}

void executeRunning() {
  static long lastTime = 0;
  long now = millis();
  if (abs(now - lastTime) > HUMIDITY_READ_DELAY) {
    lastTime = now;
    getHumidityRead();
  }
}

void updateServoMotorPosition() {
  pos = map(humAveragePercent, 0, 100, 0, 180);
  if (pos > 180) {
    pos = 180;
  } else if (pos < 0) {
    pos = 0;
  }

  servo.write(pos);
}