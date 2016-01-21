#include "main.h"


DNSServer dnsServer;
ESP8266WebServer webServer(80);
// WebSocketsServer webSocket(55555);


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
  // } else if(!MDNS.begin(name)) {
  //   Serial.println("[!] mDNS error!");
  //   fail();
  } else {
    start();
  }
}


void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  // webSocket.loop();
  // ArduinoOTA.handle();
}


void start() {
  pinMode(cold_pin, OUTPUT);
  //pinMode(warm_pin, OUTPUT);
  analogWrite(cold_pin, 1024 - cold);

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

  //MDNS.addService("http", "tcp", 80);
  //MDNS.addService("ws", "tcp", 5555);
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
  bool preset_parsed = false;
  bool network_parsed = false;
  bool hardware_parsed = false;
  DynamicJsonBuffer jsonBuffer;
  File configFile = SPIFFS.open("/config.json", "r");
  if(!configFile) {
    Serial.println("[!] Couldn't open the configuration file.");
    return false;
  }
  size_t size = configFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  if(!json.success()) {
    Serial.println("[!] Configuration parsing error.");
    return false;
  }
  // if(root.containsKey("hardware") && root["hardware"].is<JsonObject&>()) {
  //   JsonObject& hardware = root["hardware"];
  //   if (hardware.containsKey("pwm_frequency")) {
  //     if(!hardware["pwm_frequency"].is<int>()) {
  //       Serial.println("[!] JSON parsing error!");
  //       return false;
  //     }
  //     pwm_frequency = hardware["pwm_frequency"];
  //     Serial.printf("PWM frequency: %d Hz\n", pwm_frequency);
  //     analogWriteFreq(pwm_frequency);
  //   }
  //   if(hardware.containsKey("gpio") && hardware["gpio"].is<JsonObject&>()) {
  //     JsonObject& gpio = hardware["gpio"];
  //     for (JsonObject::iterator it=gpio.begin(); it!=gpio.end(); ++it) {
  //       Serial.println();
  //       Serial.printf("GPIO: %s\n", it->key);
  //       if(it->value.is<JsonObject&>()) {
  //         JsonObject& trit = it->value;
  //         for (JsonObject::iterator dit=trit.begin(); dit!=trit.end(); ++dit) {
  //           Serial.printf("  %s: ", dit->key);
  //           Serial.println(dit->value.asString());
  //         }
  //       }
  //     }
  //   }
  // } else { Serial.println("[!] Err2"); return false; }
  //  IPAddress ip;
  // ip.fromString("10.1.2.3");
  // Serial.println(ip);
  for (auto root : json) {
    if (strcmp(root.key, "network") == 0) {
      if ((!root.value.is<JsonObject&>()) || network_parsed) return false;
      Serial.println("notimplementedyet");
      network_parsed = true;
    } else if (strcmp(root.key, "hardware") == 0) {
      if ((!root.value.is<JsonObject&>()) || hardware_parsed) return false;
      for (auto hardware : root.value.as<JsonObject&>()) {
        if (strcmp(hardware.key, "pwm_frequency") == 0) {
          if (!hardware.value.is<int>()) return false;
          pwm_frequency = hardware.value;
          analogWriteFreq(pwm_frequency);
        } else if (strcmp(hardware.key, "gpio") == 0) {
          if (!hardware.value.is<JsonObject&>()) return false;
          for (auto gpio : hardware.value.as<JsonObject&>()) {
            io current_gpio;
            if (!gpio.value.is<JsonObject&>()) return false;
            if (gpios.count((char*)(const char*)gpio.key) != 0) return false;
            if (!gpio.value.as<JsonObject&>().containsKey("pin")) return false;
            if (!gpio.value.as<JsonObject&>().containsKey("mode")) return false;
            for (auto item : gpio.value.as<JsonObject&>()) {
              if (strcmp(item.key, "pin") == 0) {
                if (!item.value.is<int>()) return false;
                current_gpio.pin = item.value;
              } else if (strcmp(item.key, "mode") == 0) {
                if (!item.value.is<char*>()) return false;
                if (strcmp(item.value, "in") == 0) {
                  current_gpio.mode = INPUT;
                } else if (strcmp(item.value, "out") == 0) {
                  current_gpio.mode = OUTPUT;
                } else { return false; }
                pinMode(current_gpio.pin, current_gpio.mode);
              } else if (strcmp(item.key, "store") == 0) {
                if (!item.value.is<bool>()) return false;
                if (current_gpio.mode == INPUT) return false;
                current_gpio.store = item.value;
              } else if (strcmp(item.key, "inverting") == 0) {
                if (!item.value.is<bool>()) return false;
                current_gpio.inverting = item.value;
              } else if (strcmp(item.key, "enabled") == 0) {
                if (!item.value.is<bool>()) return false;
                current_gpio.enabled = item.value;
              } else { return false; }
            }
            if (current_gpio.enabled) {
              //gpios[(char*)gpio.key] = current_gpio;
            }
            current_gpio = {};
            // gpios.at(key)
          }
        } else { return false; }
      }
      hardware_parsed = true;
    } else if (strcmp(root.key, "preset") == 0) {
      if ((!root.value.is<JsonObject&>()) || preset_parsed) return false;
      Serial.println("notimplementedyet");
      preset_parsed = true;
    } else { return false; }
  }
  configFile.close();
  return true;
}


bool config_write() {
  DynamicJsonBuffer jsonBuffer;
  File configFile = SPIFFS.open("/config.json", "w");
  if(!configFile) return false;
  JsonObject& root = jsonBuffer.createObject();
                  root["cold"] = cold;
  root.prettyPrintTo(configFile);
  configFile.close();
  return true;
}


void handleRoot() {
  String uri = webServer.uri();
  if(webServer.args() > 0) {
    httpInterface();
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
//
// void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
//   switch(type) {
//     case WStype_DISCONNECTED:
//         break;
//     case WStype_CONNECTED:
//         IPAddress remote_ip = webSocket.remoteIP(num);
//         // printf "[%u] Connected from %d.%d.%d.%d url: %s\n", num, remote_ip[0], remote_ip[1], remote_ip[2], remote_ip[3], payload);
//         webSocket.sendTXT(num, "Connected");
//         break;
//     case WStype_TEXT:
//         // printf("[%u] get Text: %s\n", num, payload);
//         if(payload[0] == '#') {
//             uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);
//             analogWrite(LED_RED,    ((rgb >> 16) & 0xFF));
//             analogWrite(LED_GREEN,  ((rgb >> 8) & 0xFF));
//             analogWrite(LED_BLUE,   ((rgb >> 0) & 0xFF));
//         }
//         break;
//   }
// }

void httpInterface() {
  String value = webServer.arg("value");
  if(value == "get") {
    webServer.send(200, "text/plain", String(cold));
  } else {
    cold = value.toInt();
    analogWrite(cold_pin, 1024 - cold);
    webServer.send(200, "text/plain", "OK");
    config_write();
  }
}
