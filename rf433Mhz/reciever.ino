#include <RH_ASK.h>
#include <SPI.h> 

RH_ASK driver;
const int ledPin =  13;
int state = HIGH;
int led_status=0;


void setup()
{
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);  
    if (!driver.init())
         Serial.println("init failed");
}

void led(){

  if (led_status == 0)
    state = HIGH;
  else
    state = LOW;      
  
  digitalWrite(ledPin, state);  
}

void loop()
{
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) 
    {
  int i;

  Serial.println(buflen);
  led_status = buflen;
  Serial.println(led_status);
  led();
  
    }
}