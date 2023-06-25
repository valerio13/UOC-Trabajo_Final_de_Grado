//
//  Autor: Valerio Colantonio
//

#include "ble.h"
#include "config.h"
#include <Arduino.h>

// Función de setup del programa
void setup() {
  // Inicialización de pines de salida
  pinMode(OUTPUT_0, OUTPUT);
  pinMode(OUTPUT_1, OUTPUT);
  pinMode(OUTPUT_2, OUTPUT);
  pinMode(OUTPUT_3, OUTPUT);

  digitalWrite(OUTPUT_0, HIGH);
  digitalWrite(OUTPUT_1, HIGH);
  digitalWrite(OUTPUT_2, HIGH);
  digitalWrite(OUTPUT_3, HIGH);

  Serial.begin(921600);
  bleSetup();
}

// Función de loop del programa
void loop() { bleLoop(); }