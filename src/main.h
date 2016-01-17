#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
// // #include <WiFiUdp.h>
// // #include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <FS.h>

#define MAGIC 0x2BEC6 // must be 3 bytes or less
#define DARK_MAGIC 0xDEFEC8 // must be 3 bytes or less

void setup();
void loop();

void fail();
void start();
void streamFile(String filename);
void handleRoot();
bool config_read();
bool config_write();
void setValue(String value);

IPAddress ap_ip;
IPAddress ap_gw;
IPAddress ap_dns;
IPAddress ap_subnet;

IPAddress sta_ip;
IPAddress sta_gw;
IPAddress sta_dns;
IPAddress sta_subnet;

char* ap_essid;
char* ap_password;

char* sta_essid;
char* sta_password;

char* mode;

int channel;
int tx_power;

int warm_pin;
int warm;

int cold_pin = 2;
int cold = 0;
