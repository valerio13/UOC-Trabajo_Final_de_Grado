#ifndef BLE_HUMIDITY_H
#define BLE_HUMIDITY_H

#include <Arduino.h>

void setupBleHumidity();
int getBleHumidityData();
String getHumidityCalibData(String bleCalibCharacteristic);
bool setHumidityCalibData(String bleCalibCharacteristic);
bool connectToHumidityServer();
void disconnectFromHumidityServer();

#endif