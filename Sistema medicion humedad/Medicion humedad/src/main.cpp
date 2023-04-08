#include <Arduino.h>
#include <ESP32Servo.h>

const int potPin = 34;
const int servoPin = 18;

Servo servo;

// variable for storing the potentiometer value
int potValue = 0;
int pos = 0;

int minValue = 9999;
int maxValue;

const int NUM_VALORES =
    10; // Constante que define el número de valores que se promediarán
int valores[NUM_VALORES]; // Array que almacena los valores
int indice = 0; // Índice del array donde se almacenará el siguiente valor
int suma = 0;   // Variable que almacena la suma de los valores

int getMedia(int newValue) {
  valores[indice] = newValue; // Almacenamiento del valor en el array
  indice++;                   // Incremento del índice para el siguiente valor

  if (indice >= NUM_VALORES) { // Si se han almacenado todos los valores
    indice = 0;                // Reinicio del índice para volver a empezar
  }

  suma = 0; // Reinicio de la variable que almacena la suma de los valores

  for (int i = 0; i < NUM_VALORES;
       i++) {           // Bucle que recorre todos los valores del array
    suma += valores[i]; // Acumulación de los valores en la variable suma
  }

  int media = suma / NUM_VALORES;
  return media;
}

void setup() {
  servo.attach(servoPin, 500, 2400);

  Serial.begin(921600);
  delay(1000);
}

void loop() {
  // Reading potentiometer value
  int inRead = analogRead(potPin);

  potValue = getMedia(inRead);
  Serial.print("potValue: ");
  Serial.println(potValue);

  pos = map(potValue, 0, 4095, 0, 180);
  Serial.println(pos);
  if (pos > 180) {
    pos = 180;
  } else if (pos < 0) {
    pos = 0;
  }
  Serial.println(pos);

  servo.write(pos);

  minValue = min(potValue, minValue);
  maxValue = max(potValue, maxValue);

  Serial.println("");
  delay(500);
}
