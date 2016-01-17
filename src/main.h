#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
// // #include <WiFiUdp.h>
// // #include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <FS.h>

void setup();
void loop();

void fail();
void start();
void streamFile(String filename);
void handleRoot();
bool config_read();
bool config_write();
void setBrightness(String value);

int brightness;
int brightness_pin = 2;
