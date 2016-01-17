#include "main.h"


File configFile;
DNSServer dnsServer;
ESP8266WebServer webServer(80);
IPAddress apIP(192, 168, 1, 1);


void setup() {
  analogWriteFreq(100);
  Serial.begin(115200);
  Serial.println();

  if(!SPIFFS.begin()) {
    Serial.println("[!] Filesystem error!");
    fail();
  } else if(!config_read()) {
    Serial.println("[!] Configuration error!");
    start();  // fail();
  } else {
    start();
  }
  // Serial.setDebugOutput(true);
  // ArduinoOTA.begin();

  pinMode(brightness_pin, OUTPUT);
  analogWrite(brightness_pin, brightness);

}


void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  // ArduinoOTA.handle();
}


void start() {
  WiFi.mode(WIFI_AP_STA);
  // WiFi.begin(sta_essid, sta_password);
  // WiFi.config(sta_ip, sta_gw, sta_subnet, sta_dns);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Luz", NULL, 6); // 	WiFi.softAP(ssid, password, channel, hide_ssid);

  dnsServer.start(53, "*", apIP);

  webServer.onNotFound(handleRoot);
  webServer.on("/", handleRoot);
  webServer.begin();
}


void fail() {
  Serial.println("[!] Halted!");
  while(1) delay(1);
}


bool config_read() {
  DynamicJsonBuffer jsonBuffer;
  configFile = SPIFFS.open("/config.json", "r");
  if(!configFile) {
    Serial.println("NOT CONFIGFILE... Something bad happens...");
    return false;
  }
  size_t size = configFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  JsonObject& root = jsonBuffer.parseObject(buf.get());
  if(!root.success()) {
    Serial.println("Non-success ¿why?");
    return false;
  }
    brightness = root["brightness"];
    Serial.print("json read:");
    Serial.println(brightness);
  return true;
}


bool config_write() {
  DynamicJsonBuffer jsonBuffer;
  configFile = SPIFFS.open("/config.json", "w");
  if(!configFile) return false;
  JsonObject& root = jsonBuffer.createObject();
                  root["brightness"] = brightness;
                  Serial.print("json write:");
                  Serial.println(brightness);
  root.printTo(configFile);
  return true;
}


void handleRoot() {
  String uri = webServer.uri();
  if(webServer.hasArg("value")) {
    setBrightness(webServer.arg("value"));
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


void setBrightness(String value) {
  if(value == "get") {
    webServer.send(200, "text/plain", String(brightness));
  } else {
    brightness = value.toInt();
    analogWrite(brightness_pin, 1024-brightness);
    webServer.send(200, "text/plain", "OK");
    config_write();
  }
}
