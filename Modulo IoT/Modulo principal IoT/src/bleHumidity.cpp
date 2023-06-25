/*
//  Módulo IoT: BLE client
//  Gestión de la comunicación BLE con el módulo de medición de humedad
*/

#include "config.h"
#include "mqttService.h"
#include <Arduino.h>
#include <BLEDevice.h>

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

static BLERemoteCharacteristic *pRemoteHumCharacteristic;
static BLERemoteCharacteristic *pRemoteHumCalibCharacteristic;
static BLERemoteCharacteristic *pRemoteDryCalibCharacteristic;

static BLEAdvertisedDevice *myDevice;

// Callback que se ejecuta en caso de conexión/desconexión
class HumidityClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {}
  void onDisconnect(BLEClient *pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

// Función de conexión con el módulo de humedad que funciona como servidor
bool connectToHumidityServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  // Se crea el cliente para conectarse al servidor
  BLEClient *pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new HumidityClientCallback());
  Serial.println(" - Created client");
  pClient->connect(myDevice);
  Serial.println(" - Connected to server");

  BLERemoteService *pRemoteService = pClient->getService(HUM_SERVICE_UUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(HUM_SERVICE_UUID);
    pClient->disconnect();
    return false;
  }

  Serial.println(" - Found our service");
  pRemoteHumCharacteristic = pRemoteService->getCharacteristic(HUM_CHAR_UUID);
  pRemoteHumCalibCharacteristic =
      pRemoteService->getCharacteristic(MAX_CALIB_HUM_CHAR_UUID);
  pRemoteDryCalibCharacteristic =
      pRemoteService->getCharacteristic(MAX_CALIB_DRYNESS_HUM_CHAR_UUID);

  if (pRemoteHumCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(HUM_CHAR_UUID);
    pClient->disconnect();
    return false;
  } else if (pRemoteHumCalibCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(MAX_CALIB_HUM_CHAR_UUID);
    pClient->disconnect();
    return false;
  } else if (pRemoteDryCalibCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(MAX_CALIB_DRYNESS_HUM_CHAR_UUID);
    pClient->disconnect();
    return false;
  }

  Serial.println(" - Found all characteristics");
  // Lectura de la característica de humedad
  if (pRemoteHumCharacteristic->canRead()) {
    std::string value = pRemoteHumCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }
  connected = true;
  return true;
}

// Callback que se ejecuta cuando el dispositivo ha sido encontrado
class HumidityAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (advertisedDevice.getName() == BLE_HUM_SERVER_NAME) {
      Serial.println("conección inicial");
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

// Función de setup
void setupBleHumidity() {
  Serial.println("Starting BLE Humidity Client...");
  BLEDevice::init("");

  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(
      new HumidityAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

// Función de lectura de humedad
int getBleHumidityData() {
  // Se conecta primero con el servidor
  if (doConnect == true) {
    if (connectToHumidityServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin "
                     "more we will do.");
    }
    doConnect = false;
  }

  int value = 0;

  if (connected) {
    if (pRemoteHumCharacteristic->canRead()) {
      // Lectura de la característica de humedad
      value = pRemoteHumCharacteristic->readUInt16();
      Serial.print("Humedad: ");
      Serial.println(value);

      publishMqttData(value);
    }
  } else if (doScan) {
    Serial.println("Humidity: Not connected");
    BLEDevice::getScan()->start(0);
  }
  return value;
}

// Lee el estado de la calibración
String getHumidityCalibData(String bleCalibCharacteristic) {
  Serial.println(bleCalibCharacteristic.c_str());

  if (doConnect == true) {
    if (connectToHumidityServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server.");
    }
    doConnect = false;
  }

  std::string value = "---";

  if (connected) {
    if (bleCalibCharacteristic == MAX_CALIB_HUM_CHAR_UUID) {
      if (pRemoteHumCalibCharacteristic->canRead()) {
        value = pRemoteHumCalibCharacteristic->readValue();
      }
    } else if (bleCalibCharacteristic == MAX_CALIB_DRYNESS_HUM_CHAR_UUID) {
      if (pRemoteDryCalibCharacteristic->canRead()) {
        value = pRemoteDryCalibCharacteristic->readValue();
      }
    }
    Serial.print(bleCalibCharacteristic.c_str());
    Serial.println(value.c_str());
  } else if (doScan) {
    BLEDevice::getScan()->start(0);
  }

  return String(value.c_str());
}

// Inicia la calibración de humedad o sequedad
bool setHumidityCalibData(String bleCalibCharacteristic) {
  Serial.print("setHumidityCalibData: ");
  Serial.println(bleCalibCharacteristic.c_str());

  if (doConnect == true) {
    if (connectToHumidityServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server.");
    }
    doConnect = false;
  }

  bool value = false;

  if (connected) {
    Serial.println("setHumidityCalibData: connected");
    if (bleCalibCharacteristic == MAX_CALIB_HUM_CHAR_UUID) {
      if (pRemoteHumCalibCharacteristic->canRead()) {
        pRemoteHumCalibCharacteristic->writeValue(START_CALIB);
        value = true;
        Serial.println("->writeValue(START_CALIB)");
      }
    } else if (bleCalibCharacteristic == MAX_CALIB_DRYNESS_HUM_CHAR_UUID) {
      if (pRemoteDryCalibCharacteristic->canRead()) {
        pRemoteDryCalibCharacteristic->writeValue(START_CALIB);
        value = true;
        Serial.println("->writeValue(START_CALIB)");
      }
    }
  } else if (doScan) {
    Serial.println("BLEDevice::getScan()->start(0)");
    BLEDevice::getScan()->start(0);
  }

  return value;
}