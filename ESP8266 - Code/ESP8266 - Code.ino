#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

// define
const int ledr = 4;
const int ledg = 5;
const int ledb = 16;

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);


void handleRoot() {
  char msg[] = "<!DOCTYPE html>\n"
"<html lang=\"en\" dir=\"ltr\">\n"
"  <head>\n"
"   <meta charset=\"utf-8\">\n"
"      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">\n"
"   <title>ColorPicker</title>\n"
"   <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css\" integrity=\"sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh\" crossorigin=\"anonymous\">\n"
"\n"
" </head>\n"
" <body>\n"
"   <div class=\"container\">\n"
"     <br><br>\n"
"     <input type=\"range\" min=\"0\" max=\"255\" value=\"50\" class=\"slider container-fluid\" id=\"r\"><br><br>\n"
"     <input type=\"range\" min=\"0\" max=\"255\" value=\"50\" class=\"slider container-fluid\" id=\"g\"><br><br>\n"
"     <input type=\"range\" min=\"0\" max=\"255\" value=\"50\" class=\"slider container-fluid\" id=\"b\"><br><br>\n"
"\n"
"     <div class=\"container\" id=\"status\"></div><br><br>\n"
"\n"
"     <input type=\"button\" class=\"btn btn-primary container-fluid\" id=\"btn\" value=\"Random Color\">\n"
"   </div>\n"
" </body>\n"
" <script type=\"text/javascript\">\n"
"   var r = document.getElementById('r');\n"
"   var g = document.getElementById('g');\n"
"   var b = document.getElementById('b');\n"
"   var btn = document.getElementById('btn');\n"
"   var color = document.body;\n"
"\n"
"   r.addEventListener('input', changetrigger);\n"
"   g.addEventListener('input', changetrigger);\n"
"   b.addEventListener('input', changetrigger);\n"
"   btn.addEventListener('click', () => {\n"
"     r.value = Math.floor(Math.random() * 255);\n"
"     g.value = Math.floor(Math.random() * 255);\n"
"     b.value = Math.floor(Math.random() * 255);\n"
"     changetrigger();\n"
"   });\n"
"\n"
"   changetrigger();\n"
"\n"
"   function changetrigger(){\n"
"     var data = `rgb(${r.value}, ${g.value}, ${b.value})`;\n"
"     color.style.backgroundColor = data;\n"
"\n"
"     // send to api\n"
"     var xhttp = new XMLHttpRequest();\n"
"     xhttp.onreadystatechange = function() {\n"
"       if (this.readyState == 4 && this.status == 200) {\n"
"         document.getElementById(\"status\").innerHTML = this.responseText;\n"
"       }\n"
"     };\n"
"     xhttp.open(\"GET\", `led?r=${r.value}&g=${g.value}&b=${b.value}`, true);\n"
"     xhttp.send();\n"
"   }\n"
"\n"
" </script>\n"
"</html>\n";

  
  server.send(200, "text/html", msg);
}

void handleLed() {
  int r,g,b = 0;
  if (server.args() == 3) {
    r = server.arg(0).toInt();
    g = server.arg(1).toInt();
    b = server.arg(2).toInt();

    analogWrite(ledr, r);
    analogWrite(ledg, g);
    analogWrite(ledb, b);
    
    String message = "rgb(";
    message += r;
    message += ", ";
    message += g;
    message += ", ";
    message += b;
    message += ")";

    server.send(200, "text/html", message);
  }
}

void setup() {
  // basics
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.print("Configuring access point...");

  // gpio
  pinMode(ledr, OUTPUT);
  pinMode(ledg, OUTPUT);
  pinMode(ledb, OUTPUT);

  // WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("ColorPicker");

  // DNS
  dnsServer.start(DNS_PORT, "*", apIP);
  

  // show
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/led", handleLed);
  server.onNotFound(handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() { 
  dnsServer.processNextRequest();
  server.handleClient();
}
