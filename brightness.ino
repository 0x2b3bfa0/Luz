#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char *ssid = "WLAN_8266";
const char *password = "Espressif";
const String webpage =  R"=====(<html>   <head>     <title>Brillo</title>     <script>         function ajaxRequest(){          var activexmodes=["Msxml2.XMLHTTP", "Microsoft.XMLHTTP"] //activeX versions to check for in IE          if (window.ActiveXObject){ //Test for support for ActiveXObject in IE first (as XMLHttpRequest in IE7 is broken)           for (var i=0; i<activexmodes.length; i++){            try{             return new ActiveXObject(activexmodes[i])            }            catch(e){             //suppress error            }           }          }          else if (window.XMLHttpRequest) // if Mozilla, Safari etc           return new XMLHttpRequest()          else           return false         }          function report(value) {           var http=new ajaxRequest()           http.onreadystatechange=function(){            if (http.readyState==4){             if (http.status==200 || window.location.href.indexOf("http")==-1){              // document.getElementById("result").innerHTML=http.responseText             }             else{              alert("An error has occured making the request")             }            }           }           http.open("GET", "value?brightness="+value, true)           http.send(null)         }     </script>   </head>    <body>     <style>       body {           background: #333 }       section {           background: #363;           color: #bbb;           height: 5em;           width: 50%;           border-radius: 2em;           padding: 2em;           position: absolute;           top: 50%;           left: 50%;           margin-right: -50%;           transform: translate(-50%, -50%) }       section p {           font-weight: bold;           font-family: sans-serif;           font-kerning: none;           margin-top: 0em;           font-size: 2em;           text-align: center }       section form {           margin: 0 auto;           text-align: center }       section input{           width: 75% }     </style>      <section>       <p>Brillo</p>       <form>         <input id="brightness" type="range" value=0 min=0 max=1023 onchange="report(this.value)">       </form>     </section>   </body> </html> )=====";

ESP8266WebServer server(80);

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
	server.send(200, "text/html", webpage);
}

void value() {
  String result = server.arg("brightness");
  int number;
  
  if(isNumber(result)) {
    number = result.toInt();
    Serial.println(number);
    server.send(200, "text/html", "OK!");
  } else {
    server.send(200, "text/html", "FAIL!");
  }
}

void setup() {
	delay(1000);
	Serial.begin(115200);

  // pinMode(8, OUTPUT);

	WiFi.softAP(ssid, password);

	server.on("/", root);
  server.on("/value", value);

	server.begin();
}

void loop() {
	server.handleClient();
}
