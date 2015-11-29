#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <FS.h>

#define PIN D8
#define DEBUG

//IPAddress apIP(192, 168, 1, 128);
ESP8266WebServer webServer(80);
//DNSServer dnsServer;

const char *ssid = "Lampara";
const char *password = "Espressif";

boolean isNumber(String str){
   boolean isNum=false;
   for(byte i=0;i<str.length();i++)
   {
       isNum = isDigit(str.charAt(i));
       if(!isNum) return false;
   }
   return isNum;
}

void root() {
  String result = webServer.arg("brightness");
  int value;
  
  Serial.println("Req... 0");

  if(isNumber(result)) {
    value = result.toInt();
  }
  
  if(value < 1024) {
    Serial.println("Req... 1");
    map(value, 0, 100, 0, 1024);
    analogWrite(PIN, value);
    webServer.send(200, "text/html", "<h2>Brightness successfully set</h2>");
  } else if(result == "-1") {
    Serial.println("Req... 2");
    webServer.send(200, "text/html", "<h1>Really are you trying to exploit my device?</h1><br></br><a>crushedice2000@gmail.com</a>");
  } else {
    Serial.println("Req... 3");
    File file = SPIFFS.open("/index.html", "r");
    webServer.streamFile(file, "text/html");
    file.close();
  }
}

void setup() {
	delay(100);
	Serial.begin(115200);
  //SPIFFS.begin();
    
  pinMode(PIN, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.println("Light 0.50");
  Serial.println();

  #ifdef DEBUG
    Serial.print("Starting AccessPoint... ");
  #endif
  
  //WiFi.mode(WIFI_AP);
  //WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP(ssid, password);

  #ifdef DEBUG
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("[OK] [");
    Serial.print(myIP);
    Serial.println("]");
    Serial.print("Starting DNS server...  ");
  #endif

  // dnsServer.start(53, "*", apIP);

  #ifdef DEBUG
    Serial.println("[OK]");
    Serial.print("Starting web server...  ");
  #endif
  
	webServer.on("/", root);
  webServer.onNotFound(root);
	webServer.begin();

  #ifdef DEBUG
    Serial.println("[OK]");
  #endif
 }

void loop() {
	webServer.handleClient();
}
