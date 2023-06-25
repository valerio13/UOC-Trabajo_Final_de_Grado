//
//  Autor: Valerio Colantonio
//  Módulo de lectura de humedad
//  Gestión de la humedad
//

#ifndef HUMIDITY_H
#define HUMIDITY_H

extern int humAveragePercent;

void getHumidityRead();
void humiditySetup();
bool calibrateMaxHumidity();
bool calibrateMaxDryness();

#endif