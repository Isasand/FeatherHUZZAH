#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 2     // pin DHT22 is conected to
#define DHTTYPE DHT22   

DHT dht(DHTPIN, DHTTYPE);

float get_temp(){

    float t = dht.readTemperature();
    return t;
}

float get_humid(){
   
    float h = dht.readHumidity();
    return h;
}
// Update these with values suitable for your network.
const char* ssid = "";
const char* password = "";

const char* mqtt_server = "192.168.43.16";
// constants won't change. They're used here to set pin numbers:
const int buttonPin = 16;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

int buttonState = 0; 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

char msg[50];
char temp[50];

int value = 0;

void button(){
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
     buttonState = HIGH;
     
  } else {
      buttonState = LOW;
      client.publish("outTopic1", "1");
      delay(1000);
  }
}


void setup() {
 // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}



void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void callback(char* topic, byte* payload, unsigned int length) {

  /*Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);

  }
  Serial.println();*/
  // Switch on the LED if an 1 was received as first character

  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level

    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH

  }



}



void reconnect() {

  // Loop until we're reconnected

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "Det fungerar!!!");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {

      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying

      delay(5000);
    }
  }
}

void loop() {
  snprintf (temp, 75, "%f", get_temp());
  button();  
  
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

}
