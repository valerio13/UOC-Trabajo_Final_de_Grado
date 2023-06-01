#ifndef BLE_H
#define BLE_H

#include <Arduino.h>

void setupBle();
int getBleData();
String getCalibData(String bleCalibCharacteristic);
bool setCalibData(String bleCalibCharacteristic);
#endif