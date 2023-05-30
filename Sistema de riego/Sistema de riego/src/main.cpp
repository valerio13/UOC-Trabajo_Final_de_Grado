#include "ble.h"
#include <Arduino.h>

void setup() {
  Serial.begin(921600);
  bleSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  bleLoop();
}