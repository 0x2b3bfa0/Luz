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


void handleRoot() {
  File file;
  int pwm_value;
  String brightness = webServer.arg("brightness");
  
  pwm_value = brightness.toInt();

  if(pwm_value <= 100 && pwm_value > 0 || brightness == "0") {
    pwm_value = map(pwm_value, 0, 100, 0, 1024);
    webServer.send(200, "text/plain", "OK");
    analogWrite(PIN, pwm_value);
  } else if(brightness == "-1" || brightness == "'") {
    file = SPIFFS.open("/easter_egg.html", "r");
    webServer.streamFile(file, "text/html");
    file.close();
  } else {
    file = SPIFFS.open("/index.html", "r");
    webServer.streamFile(file, "text/html");
    file.close();
  }
}

// for file in `ls -A1`; do curl -F "file=@$PWD/$file" esp8266fs.local/; done
void handleUpload() {
  File file;
  HTTPUpload& upload = webServer.upload();
  if(upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/" + filename;
    file = SPIFFS.open(filename, "w");
  } else if(upload.status == UPLOAD_FILE_WRITE) {
    if(file)
      file.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END) {
    if(file)
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

  webServer.on("/", handleRoot);
  webServer.onNotFound(handleRoot);
  webServer.onFileUpload(handleUpload);
	webServer.begin();
 }


void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
