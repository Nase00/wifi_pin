// Partially adapted from https://github.com/bbx10/WebServer_tng/blob/master/examples/AdvancedWebServer/AdvancedWebServer.ino

#ifdef ESP8266
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
ESP8266WebServer server(80);
#else
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
WebServer server(80);
#endif

#include "secrets.h"

char passcode[64] = "";
int pin;
int timeout = 150;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

#ifdef ESP8266
  if (MDNS.begin("esp8266")) {
#else
  if (MDNS.begin("esp32")) {
#endif
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  const char *headerkeys[] = { "passcode", "pin", "timeout" };
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
  server.collectHeaders(headerkeys, headerkeyssize);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  logDeviceData();

  server.handleClient();

  delay(500);
}

void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  String content = "Successfully connected to device. ";

  if (server.hasHeader("passcode") && server.hasHeader("pin")) {
    server.header("passcode").toCharArray(passcode, 64);

    pin = atoi(server.header("pin").c_str());

    if (server.hasHeader("timeout")) {
      timeout = atoi(server.header("timeout").c_str());
    }

    if (strcmp(passcode, secretPasscode) == 0) {
      content += "Access granted.";
      server.send(200, "text/html", content);

      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
      delay(timeout);
      digitalWrite(pin, LOW);
    } else {
      content += "Access denied.";
      server.send(401, "text/html", content);
    }
  } else {
    content += "Bad request.";
    server.send(500, "text/html", content);
  }

  Serial.println(content);
}

void logDeviceData() {
  Serial.print("Device IP: ");
  Serial.println(WiFi.localIP());

  Serial.print("Device MAC: ");
  Serial.println(WiFi.macAddress());

  Serial.print("Memory heap: ");
  Serial.println(ESP.getFreeHeap());
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}
