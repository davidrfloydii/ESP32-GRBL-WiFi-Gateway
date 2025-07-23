/* Simple pass through that allows clients like Universal GCode Sender to 
connect, then messages received will be passed through to the GRBL controller via the 
ESP32 serial port. */
#include <Arduino.h>
#include "secrets.h"
#include <WiFi.h>
// Setup your WIFI credentials in the secrets.h file. See README.MD for more details.
const char* ssid = WIFI_SSID;
const char* password = WIFI_PWD;
// Define both the server and the client.
WiFiServer server(23);
WiFiClient client;
// Two variables for commands one incoming and one outgoing.
String incomingLine = "";
String commandFromTelnet = "";
// Setup method connects to WiFi, and starts the telnet server.
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
