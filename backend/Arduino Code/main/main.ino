#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"  // Ensure this file contains AWS_CERT_CA, AWS_CERT_CRT, and AWS_CERT_PRIVATE as strings
#include <WiFiUdp.h>
#include <NTPClient.h>

// Set up WiFiClientSecure
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
BearSSL::WiFiClientSecure net;
PubSubClient client(net);

unsigned long lastMillis = 0;

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWiFi connected!");
}

void connectMQTT() {
  Serial.print("Connecting to MQTT...");
  int retryCount = 0;
  while (!client.connected() && retryCount < 5) {
    if (client.connect("ESP8266Client")) {  // Use a unique client ID
      Serial.println("\nMQTT connected!");
    } else {
      Serial.print(".");
      delay(1000);
      retryCount++;
    }
  }
  if (!client.connected()) {
    Serial.println("\nFailed to connect to MQTT. Restarting...");
    ESP.restart();
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  connectWiFi();

  // Configure SSL/TLS certificates
  net.setTrustAnchors(new BearSSL::X509List(AWS_CERT_CA));
  net.setClientRSACert(new BearSSL::X509List(AWS_CERT_CRT), new BearSSL::PrivateKey(AWS_CERT_PRIVATE));

  timeClient.begin();
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  connectMQTT();
}

void loop() {
  client.loop();

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!client.connected()) {
    connectMQTT();
  }

  if (!timeClient.update()) {
    Serial.println("Failed to update time. Retrying...");
    delay(500);
  }

  if (millis() - lastMillis > 5000) {
    lastMillis = millis();

    StaticJsonDocument<256> doc;
    doc["timestamp"] = timeClient.getEpochTime();
    doc["source"] = "solar";
    String weather = random(0, 2) ? "sunny" : "normal";
    doc["weather"] = weather;
    doc["units_produced"] = (weather == "sunny") ? random(80, 100) : random(10, 50);

    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    if (client.publish("energy/production", jsonBuffer)) {
      Serial.println("Published production data.");
    } else {
      Serial.println("Failed to publish production data.");
    }

    doc.clear();

    doc["timestamp"] = timeClient.getEpochTime();
    doc["units_consumed"] = random(1, 30);

    serializeJson(doc, jsonBuffer);

    if (client.publish("energy/consumption", jsonBuffer)) {
      Serial.println("Published consumption data.");
    } else {
      Serial.println("Failed to publish consumption data.");
    }
  }
}
