#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
// #include <ArduinoJson.h>
#include <DNSServer.h>
#include <FS.h>

#define LEDPIN 2

#define TXMODE 2   // {'PHY_MODE_11B': 1, 'PHY_MODE_11G': 2, 'PHY_MODE_11N': 3}
#define TXPOWER 0  // TX power in dBm
#define MACADDR {0x00, 0x00, 0x0A, 0x18, 0xA1, 0xED}

#define MODE 0    // {'STA': 0, 'AP': 1, 'STA+AP': 2}
#define AP_HIDE 0 // {'VISIBLE': 0, 'HIDDEN': 1}
#define AP_SSID "AlbaLED"
#define AP_PASS "LED-CTRL"
#define STA_SSID NULL
#define STA_PASS NULL


IPAddress apIP(192, 168, 1, 1);
ESP8266WebServer webServer(80);
DNSServer dnsServer;

const char* ssid = "AlbaLED";
const char* password = "demo-tienda";

void handleRoot();

extern "C" { 
  #include "user_interface.h"
  void __run_user_rf_pre_init(void) {
    uint8_t mac[] = MACADDR;
    system_phy_set_max_tpw(TXPOWER);
    wifi_set_phy_mode(PHY_MODE_11G);
    wifi_set_macaddr(SOFTAP_IF, &mac[0]);
  }
}


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  ArduinoOTA.begin();
  SPIFFS.begin();

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP(ssid, password);

  dnsServer.start(53, "*", apIP);

  webServer.onNotFound(handleRoot);
  webServer.on("/", handleRoot);
	webServer.begin();
}


void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  ArduinoOTA.handle();
}


void handleRoot() {
  File file;
  int pwm_value;
  String operation = webServer.arg("operation");
  
  pwm_value = operation.toInt();

  if(pwm_value <= 100 && pwm_value > 0 || operation == "0") {
    webServer.send(200, "text/plain", "OK");
    pwm_value = map(pwm_value, 0, 100, 0, 1024);
    analogWrite(LEDPIN, pwm_value);
  } else if(operation == "-1" || operation == "'") {
    file = SPIFFS.open("/easter_egg.html", "r");
    webServer.streamFile(file, "text/html");
    file.close();
  } else {
    file = SPIFFS.open("/index.html", "r");
    webServer.streamFile(file, "text/html");
    file.close();
  }
}
