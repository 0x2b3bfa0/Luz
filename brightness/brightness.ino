#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <FS.h>

#define PIN 16
#define DEBUG

IPAddress apIP(192, 168, 1, 1);
ESP8266WebServer webServer(80);
DNSServer dnsServer;

const char *ssid = "Lampara";
const char *password = "Espressif";

boolean isNumber(String str){
  boolean isNum=false;
  for(byte i=0;i<str.length();i++) {
    isNum = isDigit(str.charAt(i));
    if(!isNum) return false;
  }
  return isNum;
}

void root() {
  int value;
  File file;
  String result = webServer.arg("brightness");
  
  if(isNumber(result)) {
    value = result.toInt();
  }

 if(value <= 100) {
   value = map(value, 0, 100, 0, 1024);
   analogWrite(PIN, value);
   webServer.send(200, "text/plain", "[OK]");
 } else if(result == "-1") {
   file = SPIFFS.open("/rant.html", "r");
   webServer.streamFile(file, "text/html");
   file.close();
 } else {
   file = SPIFFS.open("/index.html", "r");
   webServer.streamFile(file, "text/html");
   file.close();
 }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  SPIFFS.begin();

  pinMode(PIN, OUTPUT);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP(ssid, password);

  dnsServer.start(53, "*", apIP);

	webServer.on("/", root);
  webServer.onNotFound(root);
	webServer.begin();
 }

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
