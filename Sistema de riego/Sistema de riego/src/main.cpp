#include "ble.h"
#include "config.h"
#include <Arduino.h>

void setup() {
  // Inicialización de pines de salida
  pinMode(OUTPUT_1, OUTPUT);
  pinMode(OUTPUT_2, OUTPUT);
  pinMode(OUTPUT_3, OUTPUT);
  pinMode(OUTPUT_4, OUTPUT);

  digitalWrite(OUTPUT_1, HIGH);
  digitalWrite(OUTPUT_2, HIGH);
  digitalWrite(OUTPUT_3, HIGH);
  digitalWrite(OUTPUT_4, HIGH);

  Serial.begin(921600);
  bleSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  bleLoop();
}