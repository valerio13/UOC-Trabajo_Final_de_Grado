/*
//  Módulo IoT: cliente MQTT
//  Gestión de la comunicación MQTT con el servidor
*/

#include "mqttService.h"
#include "networkConfig.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

WiFiClient espClient;
PubSubClient client(espClient);

// Declaración de las funciones que luego serán implementadas abajo
void setup_wifi();
void callback(char *topic, byte *message, unsigned int length);
void reconnect();

// Función de setup de la comunicación
void setupMqtt() {
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

// Función de loop
void loopMqtt() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

// Función que publica los datos en en servido
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

// Setup de la conexión Wifi
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

// Callback que se ejecuta cuando un dato viene recibido
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

// Función de reconexión
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(ID, USER, PASSCODE)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("moisture1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
