//
//  Autor: Valerio Colantonio
//  Módulo de lectura de humedad
//  Gestión de la humedad
//

#include <Preferences.h>
#include <config/humidity_config.h>
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

float getAverage(int newValue);
void printSerialMessage(const char *message, float data);
void printSerialMessage(const char *message, int data);

// Función de setup
void humiditySetup() {
  delay(1000); // TODO: borrar al final de las pruebas
  Serial.println("-----------");
  Serial.println("Start humidity setup");

  // inicializar preferences con el nombre "my-app"
  preferences.begin("my-app", false);

  // Se recuperan las calibraciones memorizadas
  maxHumidity = preferences.getInt(PREFERENCES_MAX_HUMIDITY, 1300);
  maxDryness = preferences.getInt(PREFERENCES_MAX_DRYNESS, 3400);

  printSerialMessage("Max Humidity: ", maxHumidity);

  printSerialMessage("maxDryness: ", maxDryness);

  for (int i = 0; i < MEDIA_NUM_VALUES; i++) {
    printSerialMessage("Setup n: ", i);
    getHumidityRead();
    delay(1000);
  }
  Serial.println("End humidity setup");
  Serial.println("--------");
}

// Función de calibración de la máxima humedad
bool calibrateMaxHumidity() {
  Serial.println("Calibrando humedad máxima...");

  maxHumidity = 99999;
  // Menor de 20 mediciones de humedad
  for (int i = 0; i < 20; i++) {
    int humidityRead = analogRead(HUM_SENSOR_PIN);
    Serial.println(humidityRead);
    maxHumidity = min(humidityRead, maxHumidity);
    delay(1000);
  }
  if (maxHumidity < maxDryness) {
    preferences.putInt(PREFERENCES_MAX_HUMIDITY, maxHumidity);
    Serial.println("Calibración MaxHumidity OK");
    return true;
  } else {
    Serial.println("Error calibración MaxHumidity");
    return false;
  }
}

// Función de calibración de la máxima sequedad
bool calibrateMaxDryness() {
  Serial.println("Calibrando sequedad máxima...");

  maxDryness = 0;
  // Mayor de 20 mediciones de humedad
  for (int i = 0; i < 20; i++) {
    int humidityRead = analogRead(HUM_SENSOR_PIN);
    Serial.println(humidityRead);
    maxDryness = max(humidityRead, maxDryness);
    delay(1000);
  }
  if (maxHumidity < maxDryness) {
    preferences.putInt(PREFERENCES_MAX_DRYNESS, maxDryness);
    Serial.println("Calibración maxDryness OK");
    return true;
  } else {
    Serial.println("Error calibración MaxHumidity");
    return false;
  }
}

// Lectura del input del sensor de humedad
void getHumidityRead() {
  int humidityRead = analogRead(HUM_SENSOR_PIN);
  // Cálculo de la media
  int humAverage = getAverage(humidityRead);
  // Converción a porcentaje de humedad
  humAveragePercent = (int)map(humAverage, maxDryness, maxHumidity, 0, 100);

  if (humAveragePercent > 100 || humAveragePercent < 0) {
    humAveragePercent = -1;
  }

  Serial.print("Humedad bruta: ");
  Serial.println(humidityRead);
  Serial.print("Humedad %: ");
  Serial.println(humAveragePercent);
}

// Cálculo de la media
float getAverage(int newValue) {
  values[valueIndex] = newValue; // Almacenamiento del valor en el array
  valueIndex++; // Incremento del índice para el siguiente valor

  if (valueIndex >= MEDIA_NUM_VALUES) {
    valueIndex = 0;
  }

  sum = 0; // Reinicio de la variable que almacena la suma de los valores

  for (int i = 0; i < MEDIA_NUM_VALUES; i++) {
    // Acumulación de los valores en la variable suma
    sum += values[i];
  }

  // Cálculo de la media
  float average = sum / MEDIA_NUM_VALUES;

  Serial.print("Humedad media: ");
  Serial.println(average);

  return average;
}

void printSerialMessage(const char *message, float data) {
  Serial.print(message);
  Serial.println(data);
}

void printSerialMessage(const char *message, int data) {
  Serial.print(message);
  Serial.println(data);
}