#include <Arduino.h>

const int humRaw = 34;

// variable for storing the potentiometer value
int humAverage = 0;
int minAverage = 1360;
int maxAverage = 3360;

int minValue = 9999;
int maxValue;

const int VALUES_NUM =
    10; // Constante que define el número de valores que se promediarán
int values[VALUES_NUM]; // Array que almacena los valores
int averageIndex = 0; // Índice del array donde se almacenará el siguiente valor
int sum = 0;          // Variable que almacena la suma de los valores
bool isAverageValid =
    false; // Variable que indica cuando la media ha comenzado a ser válida

int getAverage(int newValue) {
  values[averageIndex] = newValue; // Almacenamiento del valor en el array
  averageIndex++; // Incremento del índice para el siguiente valor

  if (averageIndex >= VALUES_NUM) { // Si se han almacenado todos los valores
    averageIndex = 0; // Reinicio del índice para volver a empezar
    isAverageValid = true;
  }

  sum = 0; // Reinicio de la variable que almacena la suma de los valores

  for (int i = 0; i < VALUES_NUM;
       i++) {         // Bucle que recorre todos los valores del array
    sum += values[i]; // Acumulación de los valores en la variable suma
  }

  int average = sum / VALUES_NUM;
  return average;
}

void setup() {
  Serial.begin(921600);
  delay(1000);
}

void loop() {
  int humRead = analogRead(humRaw);

  humAverage = getAverage(humRead);

  if (isAverageValid) {
    minValue = min(humAverage, minValue);
    maxValue = max(humAverage, maxValue);
  }

  Serial.print("raw;average;min;max;");
  Serial.print(humRead);
  Serial.print(";");
  Serial.print(humAverage);
  Serial.print(";");
  Serial.print(minValue);
  Serial.print(";");
  Serial.print(maxValue);

  Serial.println("");
  delay(5000);
}
