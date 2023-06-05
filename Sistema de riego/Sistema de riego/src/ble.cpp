#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <ble.h>
#include <config.h>
#include <iostream>
#include <sstream>
#include <string>

BLEServer *pServer = NULL;
BLECharacteristic *pHumudityCharacteristic;
BLECharacteristic *pCalibHumudityCharacteristic;
BLECharacteristic *pCalibDrynessCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

// extern AppState currentAppState;

void processData(std::string data);
void activateOutput(int output, int activationTime);

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
    Serial.println("onConnect");
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    Serial.println("onDisconnect");
  }
};

class StartWateringCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.print("--> Dato recibido: ");
      Serial.println(rxValue.c_str());

      processData(rxValue);
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

  // Característica calibrar máxima humedad
  pCalibHumudityCharacteristic = pService->createCharacteristic(
      START_WATERING_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE);
  pCalibHumudityCharacteristic->addDescriptor(new BLE2902());
  pCalibHumudityCharacteristic->setCallbacks(new StartWateringCallbacks());

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

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(1000); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Device disconecting - start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    Serial.println("Device conected");
    oldDeviceConnected = deviceConnected;
  }

  lastTime = millis();
}

void processData(std::string data) {
  std::stringstream ss(data);
  int output, activationTime;

  // Verificar que haya exactamente dos enteros en rxValue
  if (ss >> output && ss >> activationTime && ss.eof()) {
    // Los dos enteros fueron extraídos correctamente
    std::cout << "output: " << output << std::endl;
    std::cout << "activationTime: " << activationTime << std::endl;

    activateOutput(output, activationTime);
  } else {
    // No se encontraron dos enteros en rxValue
    std::cout << "Error: los datos recibidos no contiene dos enteros."
              << std::endl;
  }
}

void activateOutput(int output, int activationTime) {
  int selectedOutput;

  switch (output) {
  case 0:
    selectedOutput = OUTPUT_0;
    break;
  case 1:
    selectedOutput = OUTPUT_1;
    break;
  case 2:
    selectedOutput = OUTPUT_2;
    break;
  case 3:
    selectedOutput = OUTPUT_3;
    break;
  }
  std::cout << "selectedOutput: " << selectedOutput << std::endl;

  digitalWrite(selectedOutput, LOW);
  delay(activationTime * 1000);
  digitalWrite(selectedOutput, HIGH);
}