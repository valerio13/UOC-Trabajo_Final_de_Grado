//
//  Autor: Valerio Colantonio
//  Módulo de lectura de humedad
//

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

// GPIO output del servomotor
const int servoPin = 18;

bool calibratingMin = false;
bool calibratingMax = false;

// Variable que define el estado actual del módulo
AppState currentAppState = RUNNING;

// Objeto que gestiona la comunicación con el servomotor
Servo servo;

// Posición del servo
int pos = 0;

BLEServer *server;
BLECharacteristic *pHumidityCharacteristic;

const char startCalibrationChar = '1';
void executeRunning();
void updateServoMotorPosition();

// Función de setup
void setup()
{
  Serial.begin(921600);

  // Definición del output del servomotor
  servo.attach(servoPin, 500, 2400);

  humiditySetup();
  bleSetup();

  delay(1000);
}

// Función de loop
void loop() {
  if (currentAppState == RUNNING) {
    executeRunning();
  }

  bleLoop();

  delay(500);
}

// Función del estado de running
void executeRunning() {
  // Gestión del delay de la lectura de la humedad: no se usa
  // la función delay() porque bloquea la ejecución
  static long lastTime = 0;
  long now = millis();
  if (abs(now - lastTime) > HUMIDITY_READ_DELAY) {
    lastTime = now;
    // Lectura de la humedad
    getHumidityRead();
    updateServoMotorPosition();
  }
}

// Nueva posición del servomotor que indica la humedad leída
void updateServoMotorPosition() {
  pos = map(humAveragePercent, 0, 100, 0, 180);
  if (pos > 180) {
    pos = 180;
  } else if (pos < 0) {
    pos = 0;
  }

  servo.write(pos);
}