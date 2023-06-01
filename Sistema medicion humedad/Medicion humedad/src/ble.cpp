#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <ble.h>
#include <config.h>
#include <humidity.h>

BLEServer *pServer = NULL;
BLECharacteristic *pHumudityCharacteristic;
BLECharacteristic *pCalibHumudityCharacteristic;
BLECharacteristic *pCalibDrynessCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

extern AppState currentAppState;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) { deviceConnected = true; };

  void onDisconnect(BLEServer *pServer) { deviceConnected = false; }
};

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

void bleSetup() {
  Serial.begin(921600);

  // Create the BLE Device
  BLEDevice::init(BLE_DEVICE_NAME);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
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

  // Característica calibrar máxima humedad
  pCalibDrynessCharacteristic = pService->createCharacteristic(
      MAX_DRYNESS_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pCalibDrynessCharacteristic->addDescriptor(new BLE2902());
  pCalibDrynessCharacteristic->setCallbacks(new CalibDrynessCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void bleLoop() {
  if ((millis() - lastTime) < timerDelay) {
    return;
  }

  if (deviceConnected) {
    pHumudityCharacteristic->setValue(humAveragePercent);
    delay(1000);
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(1000); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }

  lastTime = millis();
}