#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <ESP32Servo.h>

#include <sstream>
#include <string>

// Definición de UUID para la característica de humedad
#define HUMIDITY_SERVICE_UUID "0000181A-0000-1000-8000-00805F9B34FB"

// #define HUMIDITY_CHARACTERISTIC_UUID "00002A6F-0000-1000-8000-00805F9B34FB"
#define HUMIDITY_CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

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
int minAverage = 1300.0;
int maxAverage = 3400.0;
int humAverage = 0.0;
int humPercentAverage = 0.0;

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
class HumidityCallback : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) {
    std::ostringstream os; // crea un str stream
    os << "Humidity: "
       << humPercentAverage;     // pone en os un string y el valor de humedad
    std::string str1 = os.str(); // convierte os en string
    Serial.println(str1.c_str());
    pCharacteristic->setValue(os.str());
  }
};

class MinHumidityCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.print("MinHumidityCallback: ");
      for (int i = 0; i < value.length(); i++) {
        Serial.println(value[i]);
      }
      Serial.println("");
    }
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    std::ostringstream os; // crea un str stream
    os << "MinHumidity: "
       << minAverage;            // pone en os un string y el valor de humedad
    std::string str1 = os.str(); // convierte os en string
    Serial.println(str1.c_str());
    pCharacteristic->setValue(os.str());
  }
};

class MaxHumidityCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    std::string value = characteristic->getValue();
    if (value.length() > 0) {
      Serial.print("MaxHumidityCallback: ");
      for (int i = 0; i < value.length(); i++) {
        Serial.println(value[i]);
      }
      Serial.println("");
    }
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    std::ostringstream os; // crea un str stream
    os << "MaxHumidity: "
       << maxAverage;            // pone en os un string y el valor de humedad
    std::string str1 = os.str(); // convierte os en string
    Serial.println(str1.c_str());
    pCharacteristic->setValue(os.str());
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
  pHumidityCharacteristic->setValue("Humidity");
  pHumidityCharacteristic->setCallbacks(new HumidityCallback());

  // Caracteristica de calibrar humedad mínima
  BLECharacteristic *minHumidityCharacteristic = pService->createCharacteristic(
      MIN_HUMIDITY_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  minHumidityCharacteristic->addDescriptor(new BLE2902());
  minHumidityCharacteristic->setCallbacks(new MinHumidityCallback());

  // Caracteristica de calibrar humedad máxima
  BLECharacteristic *maxHumidityCharacteristic = pService->createCharacteristic(
      MAX_HUMIDITY_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  maxHumidityCharacteristic->addDescriptor(new BLE2902());
  maxHumidityCharacteristic->setCallbacks(new MaxHumidityCallback());

  // Iniciar el servicio
  pService->start();

  // Iniciar el advertising
  server->getAdvertising()->start();
}

void bleLoop() {
  if (deviceConnected) {
    char cstr[16];
    itoa(humPercentAverage, cstr, 10);
    pHumidityCharacteristic->setValue(cstr);
    pHumidityCharacteristic->notify();
    delay(2000);

    Serial.println("bleLoop");
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
  int inRead = analogRead(humSensorPin);

  humAverage = getMedia(inRead);
  Serial.print("humidity: ");
  Serial.print(inRead);
  Serial.print(",");
  Serial.println(humAverage);

  pos = map(humAverage, 0, 4095, 0, 180);
  if (pos > 180) {
    pos = 180;
  } else if (pos < 0) {
    pos = 0;
  }

  servo.write(pos);

  minAverage = min(humAverage, minAverage);
  maxAverage = max(humAverage, maxAverage);
  humPercentAverage = map(humAverage, maxAverage, minAverage, 0, 100);
  if (humPercentAverage < 0) {
    humPercentAverage = 0;
  } else if (humPercentAverage > 100) {
    humPercentAverage = 100;
  }
  Serial.println("");

  bleLoop();

  delay(1000);
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

/*
TODO:
- añadir la calibración
- guardar la calibración
- dividir el código en distintos ficheros
*/