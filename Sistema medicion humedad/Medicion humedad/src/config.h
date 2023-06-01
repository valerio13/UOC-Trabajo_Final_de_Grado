#ifndef CONFIG_H
#define CONFIG_H

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define HUM_CHAR_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define MAX_HUM_CHAR_UUID "292f129c-f94f-4c8b-91b3-3f3a17f2ba0d"
#define MAX_DRYNESS_CHAR_UUID "d5db8f5f-9f28-4786-ae66-8b1e332ab298"

#define START_CALIB "c_s"
#define CALIB_ON "c_on"
#define CALIB_OK "c_ok"
#define CALIB_NOK "c_nok"

#define BLE_DEVICE_NAME "SISTEMA HUMEDAD"

// Constante que define el número de valores que se promediarán
#define MEDIA_NUM_VALUES 10
#define HUMIDITY_READ_DELAY 5000

enum AppState { RUNNING, MAX_HUM_CALIB, MAX_DRYNESS_CALIB };

#endif