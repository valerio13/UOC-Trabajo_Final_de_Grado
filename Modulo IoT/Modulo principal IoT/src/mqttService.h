/*
//  Módulo IoT: cliente MQTT
//  Gestión de la comunicación MQTT con el servidor
*/

#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

void setupMqtt();
void loopMqtt();
void publishMqttData(short value);

#endif