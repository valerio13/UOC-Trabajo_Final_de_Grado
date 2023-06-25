/*
//  Módulo IoT: BLE client
// Gestión de la comunicación BLE con el módulo de riego
*/

#ifndef BLE_IRRIGATION_H
#define BLE_IRRIGATION_H

#include <Arduino.h>

void setupBleIrrigation();
bool setIrrigationData(int output, int time);
#endif