#include "main.h"


DNSServer dnsServer;
ESP8266WebServer webServer(80);


//  IPAddress ip;
// ip.fromString("10.1.2.3");
// Serial.println(ip);

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Serial.setDebugOutput(true);

  if(!SPIFFS.begin()) {
    Serial.println("[!] Filesystem error!");
    fail();
  } else if(!config_read()) {
    Serial.println("[!] Configuration error!");
    start();  // fail();
  } else {
    start();
  }
}


void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  // ArduinoOTA.handle();
}


void start() {
  analogWriteFreq(100);

  WiFi.mode(WIFI_AP_STA);
  // WiFi.begin(sta_essid, sta_password);
  // WiFi.config(sta_ip, sta_gw, sta_subnet, sta_dns);
  WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP("Luz", NULL, 6); // 	WiFi.softAP(ssid, password, channel, hide_ssid);

  // ArduinoOTA.begin();

  dnsServer.start(53, "*", IPAddress(192, 168, 1, 1));

  webServer.onNotFound(handleRoot);
  webServer.on("/", handleRoot);
  webServer.begin();

  pinMode(cold_pin, OUTPUT);
  analogWrite(cold_pin, cold);
}


void fail() {
  ap_essid = new char[7];
  sprintf(ap_essid, "%06x", ESP.getChipId());
  digitalWrite(cold_pin, LOW);
  Serial.println(ap_essid);
  Serial.println("[!] Fatal error, crashing!");
//  while(1) delay(1);
}


bool config_read() {
  DynamicJsonBuffer jsonBuffer;
  File configFile = SPIFFS.open("/config.json", "r");
  if(!configFile) return false;
  size_t size = configFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  JsonObject& root = jsonBuffer.parseObject(buf.get());
  if(!root.success()) return false;
    cold = root["cold"];
    Serial.print("json read:");
    Serial.println(cold);
  configFile.close();
  return true;
}


bool config_write() {
  DynamicJsonBuffer jsonBuffer;
  File configFile = SPIFFS.open("/config.json", "w");
  if(!configFile) return false;
  JsonObject& root = jsonBuffer.createObject();
                  root["cold"] = cold;
                  Serial.print("json write:");
                  Serial.println(cold);
  root.prettyPrintTo(configFile);
  configFile.close();
  return true;
}


void handleRoot() {
  String uri = webServer.uri();
  if(webServer.hasArg("value")) {
    setValue(webServer.arg("value"));
  } else if(SPIFFS.exists(uri)) {
//} else if(SPIFFS.exists(uri) && uri != "/config.json") {
    streamFile(uri);
  } else {
    streamFile("/index.html");
  }
}


void streamFile(String filename) {
  String mimetype = "text/plain";
  File file = SPIFFS.open(filename, "r");
  if (filename.endsWith(".json")) mimetype = "application/json";
  else if(filename.endsWith(".html")) mimetype = "text/html";
  else if(filename.endsWith(".ico")) mimetype = "image/x-icon";
  else if(filename.endsWith(".jpg")) mimetype = "image/jpeg";
  else if(filename.endsWith(".png")) mimetype = "image/png";
  else if(filename.endsWith(".gif")) mimetype = "image/gif";
  else if(filename.endsWith(".htm")) mimetype = "text/html";
  else if(filename.endsWith(".css")) mimetype = "text/css";
  else if(filename.endsWith(".xml")) mimetype = "text/xml";
  else if(filename.endsWith(".zip")) mimetype = "application/zip";
  else if(filename.endsWith(".js")) mimetype = "application/javascript";
  webServer.streamFile(file, mimetype);
  file.close();
}


void setValue(String value) {
  if(value == "get") {
    webServer.send(200, "text/plain", String(cold));
  } else {
    cold = value.toInt();
    analogWrite(cold_pin, 1024-cold);
    webServer.send(200, "text/plain", "OK");
    Serial.println("Calling write");
    config_write();
    Serial.println("Write done");
  }
}
