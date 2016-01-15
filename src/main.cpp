#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
// #include <WiFiUdp.h>
// #include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
// #include <ArduinoJson.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <FS.h>

#include "main.h"

int config_load() {
  if(!filename.startsWith("/")) filename = '/' + filename;

  SPIFFS.begin();

  configFile = SPIFFS.open(filename, "rw");
  if (!configFile) return 1;

  size_t size = configFile.size();
  if (size > 1024) return 2;

  buf = new char[size];

  configFile.readBytes(buf.get(), size);
  json = jsonBuffer.parseObject(buf.get());
  if (!json.success()) return 3;
  return 0;
}

int config_save() {
  if (!configFile) return 1;
  json.printTo(configFile);
  return 0;
}

IPAddress apIP(192, 168, 1, 1);
ESP8266WebServer webServer(80);
DNSServer dnsServer;

// extern "C" {
//   #include "user_interface.h"
//   void __run_user_rf_pre_init(void) {
//     uint8_t mac[] = MACADDR;
//     system_phy_set_max_tpw(TXPOWER);
//     wifi_set_phy_mode(PHY_MODE_11G);
//     wifi_set_macaddr(SOFTAP_IF, &mac[0]);
//   }
// }
 #define LEDPIN 2
 #define AP_SSID "Luz"


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  // ArduinoOTA.begin();
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
  analogWriteFreq(100);

  int number = 0;
  int B;

  for (int i = sizeof(number); i > 0 ; i--){
     B = EEPROM.read(i-1);
     if (i != 1){
       B = B << (8*i);
     }
     number = number | B;
     B = 0;
   }

  analogWrite(LEDPIN, number);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP(AP_SSID); // 	WiFi.softAP(ssid, password);


  dnsServer.start(53, "*", apIP);

  webServer.onNotFound(handleRoot);
  webServer.on("/", handleRoot);
	webServer.begin();
}


void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  // ArduinoOTA.handle();
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
  if(value == "get") {
    String state = String(1024 - analogRead(LEDPIN));
    webServer.send(200, "text/plain", state);
  } else {
    int number = 1024 - value.toInt();
    analogWrite(LEDPIN, number);
    webServer.send(200, "text/plain", "OK");
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
