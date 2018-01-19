#include <ESP8266WiFi.h>
#define SERIAL_LOG true 

const char* ssid = "";
const char* pass = "";

void setup() {
  Serial.begin(115200);
  connect();
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

void ShowConnection(){
 if (WiFi.status()!=WL_CONNECTED){
  if(SERIAL_LOG){
   Serial.print("connecting.."); 
  }
  connect();
  ShowConnection();
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

void loop() {
    ShowConnection();
}