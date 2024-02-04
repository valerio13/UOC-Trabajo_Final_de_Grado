//
//  Autor: Valerio Colantonio
//  Módulo de lectura de humedad
//  Constantes del módulo
//

#ifndef MAIN_CONFIG_H
#define MAIN_CONFIG_H

// Deley en la lectura
#define HUMIDITY_READ_DELAY 5000

#define ACTIVATE_DEEP_SLEEP_AFTER_MS 60000
// Estado del módulo: running, calibrando humedad o sequedad
enum AppState { RUNNING, MAX_HUM_CALIB, MAX_DRYNESS_CALIB };

#endif