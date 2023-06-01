#include <Preferences.h>
#include <config.h>
#include <humidity.h>

Preferences preferences;

// Variables para el cálculo de la humedad
int maxHumidity;
int maxDryness;
int humPercentAverage = 0;
int values[MEDIA_NUM_VALUES]; // Array que almacena los valores
int valueIndex = 0; // Índice del array donde se almacenará el siguiente valor
int sum = 0;        // Variable que almacena la suma de los valores
int humAveragePercent = 0;

const int humSensorPin = 32;

float getAverage(int newValue);

void humiditySetup() {
  for (int i = 0; i < MEDIA_NUM_VALUES; i++) {
    getHumidityRead();
  }

  preferences.begin("my-app",
                    false); // inicializar preferences con el nombre "my-app"

  maxHumidity = preferences.getInt("maxHumidity", 1300);
  maxDryness = preferences.getInt("maxDryness", 3400);

  Serial.print("maxHumidity: ");
  Serial.println(maxHumidity);

  Serial.print("maxDryness: ");
  Serial.println(maxDryness);
}

bool calibrateMaxHumidity() {
  Serial.println("Calibrando humedad máxima...");

  maxHumidity = 99999;
  for (int i = 0; i < 20; i++) {
    int humidityRead = analogRead(humSensorPin);
    Serial.println(humidityRead);
    maxHumidity = min(humidityRead, maxHumidity);
    delay(1000);
  }
  if (maxHumidity < maxDryness) {
    preferences.putInt("maxHumidity", maxHumidity);
    Serial.println("Calibración MaxHumidity OK");
    return true;
  } else {
    Serial.println("Error calibración MaxHumidity");
    return false;
  }
}

bool calibrateMaxDryness() {
  Serial.println("Calibrando sequedad máxima...");

  maxDryness = 0;
  for (int i = 0; i < 20; i++) {
    int humidityRead = analogRead(humSensorPin);
    Serial.println(humidityRead);
    maxDryness = max(humidityRead, maxDryness);
    delay(1000);
  }
  if (maxHumidity < maxDryness) {
    preferences.putInt("maxDryness", maxDryness);
    Serial.println("Calibración maxDryness OK");
    return true;
  } else {
    Serial.println("Error calibración MaxHumidity");
    return false;
  }
}

void getHumidityRead() {
  int humidityRead = analogRead(humSensorPin);
  int humAverage = getAverage(humidityRead);
  humAveragePercent = (int)map(humAverage, maxDryness, maxHumidity, 0, 100);

  if (humAveragePercent > 100 || humAveragePercent < 0) {
    humAveragePercent = -1;
  }

  Serial.print("Humedad bruta: ");
  Serial.println(humidityRead);
  Serial.print("Humedad %: ");
  Serial.println(humAveragePercent);
}

float getAverage(int newValue) {
  values[valueIndex] = newValue; // Almacenamiento del valor en el array
  valueIndex++; // Incremento del índice para el siguiente valor

  if (valueIndex >= MEDIA_NUM_VALUES) {
    valueIndex = 0;
  }

  sum = 0; // Reinicio de la variable que almacena la suma de los valores

  for (int i = 0; i < MEDIA_NUM_VALUES;
       i++) {         // Bucle que recorre todos los valores del array
    sum += values[i]; // Acumulación de los valores en la variable suma
  }

  float average = sum / MEDIA_NUM_VALUES;

  Serial.print("Humedad media: ");
  Serial.println(average);

  return average;
}