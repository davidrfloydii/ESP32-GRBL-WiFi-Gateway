#include <Arduino.h>
#include "secrets.h"
#include <WiFi.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PWD;

WiFiServer server(23);
WiFiClient client;

String incomingLine = "";
String commandFromTelnet = "";

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 16, 17); // UART to GRBL

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Telnet server started on port 23.");
}

void loop() {
  // Accept new client if none connected
  if (!client || !client.connected()) {
    client = server.available();
    if (client) {
      Serial.println("Telnet client connected.");
    }
  }

  // Forward data from telnet client to GRBL
  while (client && client.connected() && client.available()) {
    char c = client.read();
    Serial1.write(c);
  }

  // Read line from GRBL Serial1
  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\r') continue; // ignore carriage returns
    if (c == '\n') {
      incomingLine.trim();
      if (incomingLine.length() > 0) {
        Serial.println("[GRBL] " + incomingLine); // Debug print once per line
        if (client && client.connected()) {
          client.println(incomingLine);  // Forward line to telnet client
        }
      }
      incomingLine = "";
    } else {
      incomingLine += c;
    }
  }
}
