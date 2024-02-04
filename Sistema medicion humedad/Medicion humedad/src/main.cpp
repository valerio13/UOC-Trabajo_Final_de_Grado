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
#include <config/main_config.h>
#include <humidity.h>
#include <sstream>
#include <string>

/*
  Modificaciones a realizar:
  2 - BLE como server
  3 - estado de configuración (config) y de utilizo normal (running)
*/

bool calibratingMin = false;
bool calibratingMax = false;

// Variable que define el estado actual del módulo
AppState currentAppState = RUNNING;

BLEServer *server;
BLECharacteristic *pHumidityCharacteristic;

const char startCalibrationChar = '1';
void executeRunning();
void updateServoMotorPosition();
void executeDeepSleep();

void setup()
{
  Serial.begin(921600);
  esp_sleep_enable_timer_wakeup(30000000);

  humiditySetup();
  // bleSetup();

  delay(1000);
}

// Función de loop
void loop() {
  if (currentAppState == RUNNING) {
    executeRunning();
  }

  // bleLoop();
  executeDeepSleep();

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
  }
}

void executeDeepSleep() {
  static long lastTime = 0;
  long now = millis();
  if (abs(now - lastTime) > ACTIVATE_DEEP_SLEEP_AFTER_MS) {
    Serial.println("Deep sleep");
    esp_deep_sleep_start();
  }
}