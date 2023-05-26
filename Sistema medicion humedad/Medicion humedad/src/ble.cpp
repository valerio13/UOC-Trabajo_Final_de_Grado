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
uint8_t txValue = 0;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) { deviceConnected = true; };

  void onDisconnect(BLEServer *pServer) { deviceConnected = false; }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++)
        Serial.print(rxValue[i]);

      Serial.println();
      Serial.println("*********");
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
      HUMIDITY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  pHumudityCharacteristic->addDescriptor(new BLE2902());

  // Característica calibrar máxima humedad
  pCalibHumudityCharacteristic = pService->createCharacteristic(
      MAX_HUMIDITY_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pCalibHumudityCharacteristic->addDescriptor(new BLE2902());
  pCalibHumudityCharacteristic->setCallbacks(new MyCallbacks());

  // Característica calibrar máxima humedad
  pCalibDrynessCharacteristic = pService->createCharacteristic(
      MAX_DRYNESS_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pCalibDrynessCharacteristic->addDescriptor(new BLE2902());
  pCalibDrynessCharacteristic->setCallbacks(new MyCallbacks());

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
    static char humidityTemp[6];
    dtostrf(humAveragePercent, 6, 2, humidityTemp);
    // Set humidity Characteristic value and notify connected client
    pHumudityCharacteristic->setValue(humidityTemp);
    txValue++;
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