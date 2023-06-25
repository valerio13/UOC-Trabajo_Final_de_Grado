/*
//  Módulo IoT: BLE client
// Gestión de la comunicación BLE con el módulo de riego
*/

#include "config.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <Wire.h>

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

static BLERemoteCharacteristic *pRemoteIrrigationCharacteristic;

static BLEAdvertisedDevice *myDevice;

// Callback que se ejecuta en caso de conexión/desconexión
class IrrigationClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {}
  void onDisconnect(BLEClient *pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

// Función de conexión con el servidor de riego
bool connectToIrrigationServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new IrrigationClientCallback());
  Serial.println(" - Created client");
  // Conexión con el servidor
  pClient->connect(myDevice);
  Serial.println(" - Connected to server");
  BLERemoteService *pRemoteService = pClient->getService(IRRI_SERVICE_UUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(IRRI_SERVICE_UUID);
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  pRemoteIrrigationCharacteristic =
      pRemoteService->getCharacteristic(START_WATERING_CHAR_UUID);

  if (pRemoteIrrigationCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(START_WATERING_CHAR_UUID);
    pClient->disconnect();
    return false;
  }

  Serial.println(" - Found all characteristics");
  connected = true;
  return true;
}

// Callback que se ejecuta cuando el dispositivo ha sido encontrado
class IrrigationAdvertisedDeviceCallbacks
    : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (advertisedDevice.getName() == BLE_IRRI_DEVICE_NAME) {
      Serial.println("Device encontrado");
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

// Función de setup de la comunicación
void setupBleIrrigation() {
  Serial.println("Starting BLE Irrigation Client...");
  BLEDevice::init("");

  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(
      new IrrigationAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

// Activación de la salida
bool setIrrigationData(int output, int time) {
  Serial.println("setIrriData");

  if (doConnect == true) {
    if (connectToIrrigationServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server.");
    }
    doConnect = false;
  }

  bool value = false;

  if (connected) {
    Serial.println("Connected");
    std::string result = std::to_string(output) + " " + std::to_string(time);

    pRemoteIrrigationCharacteristic->writeValue(result);
    value = true;
    Serial.print("->writeValue()");
    Serial.println(result.c_str());
  } else if (doScan) {
    Serial.println("Not connected");
    BLEDevice::getScan()->start(0);
  }

  Serial.println("End setIrriData");
  return value;
}
