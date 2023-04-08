#include <Arduino.h>
#include <ESP32Servo.h>

const int potPin = 34;
const int servoPin = 18;

Servo servo;

int pos = 0; // Posición del servomotor en grados

void setup() {
  servo.attach(servoPin, 500, 2400);

  Serial.begin(921600);
  delay(1000);
}

void loop() {
  if (pos > 180) {
    pos = 0;
  }
  Serial.println(pos);
  servo.write(pos);
  pos += 45;

  Serial.println("");
  delay(5000);
}