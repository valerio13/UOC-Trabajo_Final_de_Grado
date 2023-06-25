// File de los datos de configuración del módulo

#ifndef CONFIG_H
#define CONFIG_H

// Setup de la pantalla
#define OLED_I2C_ADDRESS 0x3C
#define OLED_SDA 21
#define OLED_SCL 22

// Setup de los botones de navegación
#define BTN_UP 16    // Up Button
#define BTN_DOWN 17  // Down Button
#define BTN_ENTER 18 // Down Button
#define BTN_ESC 19   // Down Button
#define BTN_NONE -1  // Ningún botón pulsado

#define TOTAL_NAV_BUTTONS 4

// Delay en la lectura de la humedad
#define READ_HUM_DELAY 30000

// Setup del sistema de humedad
#define BLE_HUM_SERVER_NAME "SISTEMA HUMEDAD"
#define HUM_SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define HUM_CHAR_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define MAX_CALIB_HUM_CHAR_UUID "292f129c-f94f-4c8b-91b3-3f3a17f2ba0d"
#define MAX_CALIB_DRYNESS_HUM_CHAR_UUID "d5db8f5f-9f28-4786-ae66-8b1e332ab298"

#define HUMIDITY_THRESHOLD_STR "humThresh"

#define CHECK_HUMIDITY_ENABLED "humCheckEnabled"

#define START_CALIB "c_s"
#define CALIB_ON "c_on"
#define CALIB_OK "c_ok"
#define CALIB_NOK "c_nok"

// Títilos del menú de irrigación
#define IRRIGATION_OFFSET_MENU_TITLE "Riego Offset"
#define IRRIGATION_CALIB_MENU_TITLE "Calib. riego"
#define IRRIGATION_RUN_MENU_TITLE "Probar riego"

// Se memorizan los valores de calibración
#define IRRIGATION_OFFSET_TIME_PREF "irrOffsetT"
#define IRRIGATION_RUN_TIME_PREF "irrCalibT"

// Comunicación BLE con el sistema de riego
#define BLE_IRRI_DEVICE_NAME "SISTEMA RIEGO"
#define IRRI_SERVICE_UUID "a50e3574-e50c-44e2-aaf2-d1ae1272b55a"
#define START_WATERING_CHAR_UUID "2bebc6ee-61b7-40f3-a7cb-b2cbd2627e4b"

#endif