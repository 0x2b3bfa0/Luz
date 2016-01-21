#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
// // #include <WiFiUdp.h>
// // #include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
//#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <FS.h>

#define MAGIC 0x2BEC6 // must be 3 bytes or less
#define DARK_MAGIC 0xDEFEC8 // must be 3 bytes or less

// using namespace std;

void setup();
void loop();

void fail();
void start();
bool config_read();
bool config_write();
void handleRoot();
void streamFile(String filename);
void httpInterface();
//void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);

struct io {
  bool inverting = false;
  bool enabled = false;
  bool store = false;
  int mode = OUTPUT;
  int value = 0;
  int pin;
  char* name;
} gpios[9];
int gpios_pointer = 0;

int pwm_frequency;

IPAddress ap_ip;
IPAddress ap_gw;
IPAddress ap_dns;
IPAddress ap_subnet;

IPAddress sta_ip;
IPAddress sta_gw;
IPAddress sta_dns;
IPAddress sta_subnet;

char* type;
char* name;

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
