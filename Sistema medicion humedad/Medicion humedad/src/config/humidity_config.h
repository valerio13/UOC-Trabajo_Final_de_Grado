//
//  Autor: Valerio Colantonio
//  Módulo de lectura de humedad
//  Constantes del módulo
//

#ifndef HUMIDITY_CONFIG_H
#define HUMIDITY_CONFIG_H

// Constante que define el número de valores que se promediarán
#define MEDIA_NUM_VALUES 20

// Constantes de preferencia
#define PREFERENCES_MAX_HUMIDITY "maxHumidity"
#define PREFERENCES_MAX_DRYNESS "maxDryness"

// GPIO del input del sensor de humedad
#define HUM_SENSOR_PIN 32

#endif