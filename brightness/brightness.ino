#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <FS.h>

#define PIN 16


IPAddress apIP(192, 168, 1, 1);
ESP8266WebServer webServer(80);
DNSServer dnsServer;

const char* ssid;
const char* password;

void handleRoot();


void setup() {
  Serial.begin(115200);

  // Copied from OTA example
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  // Copy end
  
  ArduinoOTA.begin();
  SPIFFS.begin();

  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, HIGH);

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
    analogWrite(PIN, pwm_value);
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
