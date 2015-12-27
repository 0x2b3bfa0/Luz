#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <FS.h>

#define LEDPIN 2
#define CFG_FILE "/config.json"

#define TXMODE 2   // {'PHY_MODE_11B': 1, 'PHY_MODE_11G': 2, 'PHY_MODE_11N': 3}
#define TXPOWER 0  // TX power in dBm
#define TXCHANNEL 7

#define MACADDR {0x00, 0x00, 0x01, 0xED, 0xA1, 0xBA}

#define MODE 0    // {'STA': 0, 'AP': 1, 'STA+AP': 2}
#define AP_HIDE 0 // {'VISIBLE': 0, 'HIDDEN': 1}
#define AP_SSID "Luz"
#define AP_PASS "demo-led"
#define STA_SSID NULL
#define STA_PASS NULL

// StaticJsonBuffer<200> jsonBuffer; // DynamicJsonBuffer jsonBuffer;
IPAddress apIP(192, 168, 1, 1);
ESP8266WebServer webServer(80);
DNSServer dnsServer;

const char* ssid = "Luz";
const char* password = "demo-led";

void handleRoot();
void streamFile();
void setBrightness();


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

//  file = SPIFFS.open(CFG_FILE, "r");
//  String line = file.readStringUntil('\n');
//JsonObject& DataFile = jsonBuffer.parseObject( line );
//for( JsonObject::iterator it = DataFile.begin(); it != DataFile.end(); ++it ) {
//  JsonObject& tmpObj = *it;
//  if( tmpObj["esp"] == "ESP_ROOM_214" ) { // or whatever distiction of ESPs
//    JsonObject& tmpCfg = tmpObj["config"];
//    ssid = tmpCfg["ssid"];
//// .. and so on
//  }

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP(ssid); // 	WiFi.softAP(ssid, password);


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


void streamFile(String filename) {
  File file = SPIFFS.open(filename, "r");
  String mimetype = "text/plain";
  if (filename.endsWith(".html")) mimetype = "text/html";
  else if(filename.endsWith(".json")) mimetype = "application/json";
  else if(filename.endsWith(".htm")) mimetype = "text/html";
  else if(filename.endsWith(".css")) mimetype = "text/css";
  else if(filename.endsWith(".png")) mimetype = "image/png";
  else if(filename.endsWith(".gif")) mimetype = "image/gif";
  else if(filename.endsWith(".jpg")) mimetype = "image/jpeg";
  else if(filename.endsWith(".ico")) mimetype = "image/x-icon";
  else if(filename.endsWith(".xml")) mimetype = "text/xml";
  else if(filename.endsWith(".zip")) mimetype = "application/zip";
  else if(filename.endsWith(".js")) mimetype = "application/javascript";
  webServer.streamFile(file, mimetype);
  file.close();
}


void setBrightness() {
  String value = webServer.arg("value");
  if(value == "1") {
    webServer.send(200, "text/plain", "1");
    digitalWrite(LEDPIN, LOW);
  } else if(value == "0") {
    webServer.send(200, "text/plain", "0");
    digitalWrite(LEDPIN, HIGH);
  } else if(value == "get") {
    int state = digitalRead(LEDPIN);
    if(state == HIGH) {
      webServer.send(200, "text/plain", "0");
    } else {
      webServer.send(200, "text/plain", "1");
    }
  } else {
    streamFile("/easter_egg.html");
  }
}


void handleRoot() {
  String uri = webServer.uri();
  if(webServer.hasArg("value")) {
    setBrightness();
  } else if(SPIFFS.exists(uri)) {
    streamFile(uri);
  } else {
    streamFile("/index.html");
  }
}
