#include "mqttService.h"
#include "networkConfig.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi();
void callback(char *topic, byte *message, unsigned int length);
void reconnect();

void setupMqtt() {
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void loopMqtt() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void publishMqttData(short value) {
  if (!client.connected()) {
    reconnect();
  }

  // Convert the value to a char array
  char humString[8];
  dtostrf(value, 1, 2, humString);
  Serial.print("Humidity: ");
  Serial.println(humString);
  client.publish("moisture1", humString);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_NAME);

  WiFi.begin(WIFI_NAME, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID, USER, PASSCODE)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("moisture1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
