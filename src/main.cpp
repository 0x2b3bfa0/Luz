#include "main.h"

void config_read() {
  configFile = SPIFFS.open("/config.json", "w+");
  size_t size = configFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  const char* brightness = json["brightness"];
  Serial.print("Level: ");
  Serial.println(brightness);
}

void config_write() {
  // configFile = SPIFFS.open("/config.json", "w");
  JsonObject& json = jsonBuffer.createObject();
  json["brightness"] = "brightnessQ";
  json.printTo(configFile);
}
#define LEDPIN 2


// extern "C" {
//   #include "user_interface.h"
//   void __run_user_rf_pre_init(void) {
//     uint8_t mac[] = MACADDR;
//     system_phy_set_max_tpw(TXPOWER);
//     wifi_set_phy_mode(PHY_MODE_11G);
//     wifi_set_macaddr(SOFTAP_IF, &mac[0]);
//   }
// }


void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  // ArduinoOTA.begin();
  if (!SPIFFS.begin()) { Serial.println("Failed to mount file system"); }
  //delay(1000);
  config_read();
  config_write();


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

  //analogWrite(LEDPIN, number);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP("Luz"); // 	WiFi.softAP(ssid, password);


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
  //} else if(SPIFFS.exists(uri) && uri != "/config.json") {
    streamFile(uri);
  } else {
    streamFile("/index.html");
  }
}
