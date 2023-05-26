#ifndef HUMIDITY_H
#define HUMIDITY_H

extern int humAveragePercent;

void getHumidityRead();
void humiditySetup();
void calibrateMaxHumidity();
void calibrateMaxDryness();

#endif