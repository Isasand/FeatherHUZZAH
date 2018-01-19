#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h> 
#include <ESP8266mDNS.h>

#define SERIAL_LOG true 
#define SIMULATED_DATA false //set to false if you want to send real DHT22 sensor data 

const char* ssid = "";
const char* pass = "";
ESP8266WebServer server(80); 
float hum;
float temp;
String webString="";

#if not SIMULATED_DATA
#include <DHT.h>
#define DHTTYPE DHT22
#define DHTPIN 2
DHT dht(DHTPIN, DHTTYPE);
#endif

void setup() {
  #if not SIMULATED_DATA
  dht.begin();
  #endif
  Serial.begin(115200);
  connect();
  server.on("/", handleRoot);
  server.on("/data", getTemp);
  server.onNotFound(handleNotFound);
  server.begin();
}


void handleRoot(){
  webString = "<html><h1>Type of sensor: DHT22</h1>\n/data for temp and humidity.</html>";
  server.send(200, "text/html", webString);
}

void handleNotFound(){
  String message = "File not found";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

 for (uint8_t i = 0; i < server.args(); i++){
  message += " " + server.argName(i) + server.arg(i) + "\n";
 }
 server.send(404, "text/plain", message);
}

void connect(){
  if (WiFi.status()!= WL_CONNECTED){
    WiFi.begin(ssid, pass);
    while(WiFi.status()!= WL_CONNECTED){
     delay(500); 
      if(SERIAL_LOG){
        Serial.print(".");
    }
   }
  }
}

void HandleConnection(){
 if (WiFi.status()!=WL_CONNECTED){
  if(SERIAL_LOG){
   Serial.print("connecting.."); 
  }
  connect();
  HandleConnection();
 }
 else {
  if (SERIAL_LOG){
   Serial.println(""); 
   Serial.println("Connected to: "); 
   Serial.println(ssid); 
   Serial.print("IP-adress"); 
   Serial.print(WiFi.localIP());
   delay(1000);
   }
 }
}


#if SIMULATED_DATA
void getTemp(){
  webString = "Temperature: 21.80 %\nHumidity: 21.40 %";
  server.send(200, "text/plain", webString);
}
#else
void getTemp(){
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  delay(1000);
  webString = "Temperature : " + String(temp) + " %";
  webString += "\nHumidity: " + String(hum) + " %"; 
  server.send(200, "text/plain", webString);
}
#endif

void loop() {
    HandleConnection();
    server.handleClient();
}
