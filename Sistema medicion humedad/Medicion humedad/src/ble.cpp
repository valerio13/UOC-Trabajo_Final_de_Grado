//
//  Autor: Valerio Colantonio
//  Módulo de lectura de humedad: BLE server
//  Gestión de la comunicación BLE con el módulo IoT
//

#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <ble.h>
#include <config/ble_config.h>
#include <config/main_config.h>
#include <humidity.h>

BLEServer *pServer = NULL;
BLECharacteristic *pHumudityCharacteristic;
BLECharacteristic *pCalibHumudityCharacteristic;
BLECharacteristic *pCalibDrynessCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;

// Variables del temporizador
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

extern AppState currentAppState;

// Callback que se ejecuta cuando un cliente se conecta
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) { deviceConnected = true; };

  void onDisconnect(BLEServer *pServer) { deviceConnected = false; }
};

// Callback que se ejecuta cuando se recibe un dato de calibrar la humedad
class CalibHumidityCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Calib Humidity: ");
      Serial.println(rxValue.c_str());

      if (rxValue == START_CALIB) {
        Serial.println("Calibrando Humidity");
        currentAppState = MAX_HUM_CALIB;
        pCharacteristic->setValue(START_CALIB);
        if (calibrateMaxHumidity()) {
          pCharacteristic->setValue(CALIB_OK);
          Serial.println("Calibrando Humidity");
          Serial.println("CALIB_OK");
        } else {
          pCharacteristic->setValue(CALIB_NOK);
          Serial.println("CALIB_NOK");
        }
        currentAppState = RUNNING;
      }
    }
  }
};

// Callback que se ejecuta cuando se recibe un dato de calibrar la sequedad
class CalibDrynessCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Calib Dryness: ");
      Serial.println(rxValue.c_str());

      if (rxValue == START_CALIB) {
        Serial.println("Calibrando Sequedad");
        pCharacteristic->setValue(START_CALIB);
        currentAppState = MAX_DRYNESS_CALIB;
        if (calibrateMaxDryness()) {
          pCharacteristic->setValue(CALIB_OK);
          Serial.println("CALIB_OK");
        } else {
          pCharacteristic->setValue(CALIB_NOK);
          Serial.println("CALIB_NOK");
        }
        currentAppState = RUNNING;
      }
    }
  }
};

// Función de setup
void bleSetup() {
  Serial.begin(921600);

  // Crear el dispositivo BLE
  BLEDevice::init(BLE_DEVICE_NAME);

  // Crear el servidor BLE
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Crear el servicio BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Característica humedad
  pHumudityCharacteristic = pService->createCharacteristic(
      HUM_CHAR_UUID, BLECharacteristic::PROPERTY_READ);
  pHumudityCharacteristic->addDescriptor(new BLE2902());

  // Característica calibrar máxima humedad
  pCalibHumudityCharacteristic = pService->createCharacteristic(
      MAX_HUM_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pCalibHumudityCharacteristic->addDescriptor(new BLE2902());
  pCalibHumudityCharacteristic->setCallbacks(new CalibHumidityCallbacks());

  // Característica calibrar máxima sequedad
  // Se diferencia entre humedad máxima y sequedad máxima porque el
  // sensor de humedad unvierte los valores (valor mas alto significa mas
  // sequedad)
  pCalibDrynessCharacteristic = pService->createCharacteristic(
      MAX_DRYNESS_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pCalibDrynessCharacteristic->addDescriptor(new BLE2902());
  pCalibDrynessCharacteristic->setCallbacks(new CalibDrynessCallbacks());

  // Iniciar el servicio
  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

// Función de loop
void bleLoop() {
  if ((millis() - lastTime) < timerDelay) {
    return;
  }

  if (deviceConnected) {
    pHumudityCharacteristic->setValue(humAveragePercent);
    delay(1000);
  }

  // Desconexión
  if (!deviceConnected && oldDeviceConnected) {
    delay(1000);
    pServer->startAdvertising();
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // Conexión
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }

  lastTime = millis();
}