#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
// // #include <WiFiUdp.h>
// // #include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <FS.h>

void streamFile();
void handleRoot();
void config_read();
void config_write();
void setBrightness();

DNSServer dnsServer;
ESP8266WebServer webServer(80);
IPAddress apIP(192, 168, 1, 1);

File configFile;
DynamicJsonBuffer jsonBuffer;
