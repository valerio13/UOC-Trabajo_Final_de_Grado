#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <ESP32Servo.h>

// Definición de UUID para la característica de humedad
#define HUMIDITY_SERVICE_UUID "0000181A-0000-1000-8000-00805F9B34FB"
#define HUMIDITY_CHARACTERISTIC_UUID "00002A6F-0000-1000-8000-00805F9B34FB"

// Definición de UUID para la característica de calibración de humedad mínima
#define MIN_HUMIDITY_CHARACTERISTIC_UUID "d5db8f5f-9f28-4786-ae66-8b1e332ab298"

// Definición de UUID para la característica de calibración de humedad máxima
#define MAX_HUMIDITY_CHARACTERISTIC_UUID "292f129c-f94f-4c8b-91b3-3f3a17f2ba0d"

const int humSensorPin = 34;
const int servoPin = 18;

bool calibratingMin = false;
bool calibratingMax = false;

Servo servo;

// variable for storing the potentiometer value
int pos = 0;

// Variables para el cálculo de la humedad
int humAverage = 0;
int minAverage = 1360;
int maxAverage = 3360;
float humidity = 0.0;

const int NUM_VALORES =
    10; // Constante que define el número de valores que se promediarán
int values[NUM_VALORES]; // Array que almacena los valores
int valueIndex = 0; // Índice del array donde se almacenará el siguiente valor
int sum = 0;        // Variable que almacena la suma de los valores

BLEServer *server;
BLECharacteristic *pHumidityCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) {
    deviceConnected = true;
    Serial.println("Dispositivo conectado");
  };

  void onDisconnect(BLEServer *server) {
    deviceConnected = false;
    Serial.println("Dispositivo desconectado");
  }
};

// Declarar las funciones de callback
class MinHumidityCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    Serial.print("MinHumidityCallback: ");
    std::string value = characteristic->getValue();
    Serial.println("value");
    bool calibrateMin = value[0];
    if (calibrateMin) {
      calibratingMin = true;
    }
  }
};

class MaxHumidityCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    Serial.print("MaxHumidityCallback: ");
    std::string value = characteristic->getValue();
    Serial.println("value");
    bool calibrateMax = value[0];
    if (calibrateMax) {
      calibratingMax = true;
    }
  }
};

void bleSetup() {
  Serial.begin(115200);

  BLEDevice::init("Sistema humedad plantas 1"); // Nombre del dispositivo

  // Se crea el servidor BLE
  server = BLEDevice::createServer();
  server->setCallbacks(new MyServerCallbacks());

  // Se crea el servicio
  BLEService *pService = server->createService(HUMIDITY_SERVICE_UUID);

  // Se crean las caracteristicas
  // Caracteristica de humedad
  pHumidityCharacteristic = pService->createCharacteristic(
      HUMIDITY_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pHumidityCharacteristic->addDescriptor(new BLE2902());

  // Caracteristica de calibrar humedad mínima
  BLECharacteristic *minHumidityCharacteristic = pService->createCharacteristic(
      MIN_HUMIDITY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  minHumidityCharacteristic->addDescriptor(new BLE2902());
  minHumidityCharacteristic->setCallbacks(new MinHumidityCallback());

  // Caracteristica de calibrar humedad máxima
  BLECharacteristic *maxHumidityCharacteristic = pService->createCharacteristic(
      MAX_HUMIDITY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  maxHumidityCharacteristic->addDescriptor(new BLE2902());
  maxHumidityCharacteristic->setCallbacks(new MaxHumidityCallback());

  // Iniciar el servicio
  pService->start();

  // Iniciar el advertising
  server->getAdvertising()->start();
}

void bleLoop() {
  if (deviceConnected) {
    char buffer[10];
    dtostrf(humidity, 3, 1, buffer);
    pHumidityCharacteristic->setValue(buffer);
    pHumidityCharacteristic->notify();
  }
}

float getMedia(int newValue) {
  values[valueIndex] = newValue; // Almacenamiento del valor en el array
  valueIndex++; // Incremento del índice para el siguiente valor

  if (valueIndex >= NUM_VALORES) { // Si se han almacenado todos los valores
    valueIndex = 0;                // Reinicio del índice para volver a empezar
  }

  sum = 0; // Reinicio de la variable que almacena la suma de los valores

  for (int i = 0; i < NUM_VALORES;
       i++) {         // Bucle que recorre todos los valores del array
    sum += values[i]; // Acumulación de los valores en la variable suma
  }

  float average = sum / NUM_VALORES;
  return average;
}

void setup() {
  servo.attach(servoPin, 500, 2400);
  bleSetup();

  Serial.begin(921600);
  delay(1000);
}

void loop() {
  // Reading potentiometer value
  int inRead = analogRead(humSensorPin);

  humidity = getMedia(inRead);
  Serial.print("humidity: ");
  Serial.println(humidity);

  pos = map(humidity, 0, 4095, 0, 180);
  Serial.println(pos);
  if (pos > 180) {
    pos = 180;
  } else if (pos < 0) {
    pos = 0;
  }
  Serial.println(pos);

  servo.write(pos);

  minValue = min(humidity, minValue);
  maxValue = max(humidity, maxValue);

  Serial.println("");

  bleLoop();

  delay(500);
  /*
    if (calibratingMin) {
      // Código para calibrar el valor de humedad mínima
      // ...
      Serial.println("Calibrando valor de humedad mínima...");
      delay(5000);
      Serial.println("Calibración completada");
      calibratingMin = false;
    } else if (calibratingMax) {
      // Código para calibrar el valor de humedad máxima
      // ...
      Serial.println("Calibrando valor de humedad máxima...");
      delay(5000);
      Serial.println("Calibración completada");
      calibratingMax = false;
    } else {
      // Actualización de la humedad y notificación BLE
      // humidity = readHumidity();
      // humidityCharacteristic.setValue(humidity);
      // BLE.poll();
    }
    */
}
