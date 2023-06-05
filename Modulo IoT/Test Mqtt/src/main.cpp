#include "mqttService.h"
#include "networkConfig.h"
#include <PubSubClient.h>
#include <WiFi.h>

long lastMsg = 0;
short humidity = 0;

void setup() {
  Serial.begin(921600);
  setupMqtt();
}

void loop() {
  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;

    humidity = random(10, 50);

    publishMqttData(humidity);
  }

  loopMqtt();
}