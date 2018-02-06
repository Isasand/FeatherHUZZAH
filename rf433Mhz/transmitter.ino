#include <RH_ASK.h>
#include <SPI.h>

RH_ASK driver;
const int buttonPin = 2; 
const int ledPin =  13;
int buttonState = 0;
int state = HIGH;     
int reading;          
int previous = LOW;    

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // Tid seadns senaste knapptryck
long debounce = 200;   // the debounce time, increase if the output flickers
int msg = 0;

void setup()
{
     pinMode(ledPin, OUTPUT);
     pinMode(buttonPin, INPUT);
    Serial.begin(9600);  
    if (!driver.init())
         Serial.println("init failed");
}

void loop()
{
    const uint8_t* data = msg;
    driver.send((uint8_t *)data, strlen(data));
    driver.waitPacketSent();
    Serial.println("Sent");
    Serial.println(msg);
    led();
    
}
void led(){
  reading = digitalRead(buttonPin);
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH)
      state = LOW;
    else
      state = HIGH;

    time = millis();    
  }
  msg = state;

 digitalWrite(ledPin, state);

  previous = reading;
  
}
