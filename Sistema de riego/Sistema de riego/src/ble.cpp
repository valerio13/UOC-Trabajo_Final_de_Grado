//
//  Autor: Valerio Colantonio
//  Módulo de riego: BLE server
//  Gestión de la comunicación BLE con el módulo IoT
//

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

// Variables del temporizador
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

// Declaración de las funciones que serán implementadas abajo
void processData(std::string data);
void activateOutput(int output, int activationTime);

// Callback que se ejecuta cuando un cliente se conecta
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

// Callback que se ejecuta cuando se recibe un dato de riego
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

// Función de setup
void bleSetup() {
  Serial.begin(921600);

  // Crea el dispositivo BLE
  BLEDevice::init(BLE_DEVICE_NAME);

  // Crea el servidor BLE
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Crea el Servicio
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Característica calibrar máxima humedad
  pCalibHumudityCharacteristic = pService->createCharacteristic(
      START_WATERING_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE);
  pCalibHumudityCharacteristic->addDescriptor(new BLE2902());
  pCalibHumudityCharacteristic->setCallbacks(new StartWateringCallbacks());

  // Start del servicio
  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

// Función de loop
void bleLoop() {
  if ((millis() - lastTime) < timerDelay) {
    return;
  }

  // Desconexión
  if (!deviceConnected && oldDeviceConnected) {
    delay(1000);
    pServer->startAdvertising();
    Serial.println("Device disconecting - start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // Conexión
  if (deviceConnected && !oldDeviceConnected) {
    Serial.println("Device conected");
    oldDeviceConnected = deviceConnected;
  }

  lastTime = millis();
}

// Función que procesa los datos recibidos: número de output que activar y
// tiempo de activación
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

// Función de activación de los outputs
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