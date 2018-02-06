#define SSID_LEN 32 //SSID och password-längd bör vara <32 bytes
#define PASS_LEN 32 //http://serverfault.com/a/45509
#define CONNECTION_STRING_LEN 256
#define LED_PIN 13
#define MESSAGE_MAX_LEN 256
#define INTERVAL 2000 //intervall för skickande av meddelande till iothub (ms)
#define DEVICE_ID "feather1"

#include <ESP8266WiFi.h> 
#include <WiFiClientSecure.h> 
#include <WiFiUdp.h>
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTUtility.h>
#include <ArduinoJson.h> 

static WiFiClientSecure sslClient;

static int interval = INTERVAL;

const char *onSuccess = "\"Successfully invoke device method\"";
const char *notFound = "\"No method found\"";

static char* connectionString = "";
static char* ssid = ""; 
static char* password = ""; 

static bool messagePending = false; 
static bool messageSending = true;

void blinkLED()
{
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
}

void initWiFi() 
{
    // Attempt to connect to Wifi network:
    Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      //hämta macadress och printa
        uint8_t mac[6];
        WiFi.macAddress(mac);
        Serial.printf("You device with MAC address %02x:%02x:%02x:%02x:%02x:%02x connects to %s failed! Waiting 10 seconds to retry.\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ssid);
        WiFi.begin(ssid, password);
        delay(10000);
    }
    Serial.printf("Connected to wifi %s.\r\n", ssid);
}

void initTime()
{
    time_t epochTime;
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    while (true)
    {
        epochTime = time(NULL);

        if (epochTime == 0)
        {
            Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
            delay(2000);
        }
        else
        {
            Serial.printf("Fetched NTP epoch time is: %lu.\r\n", epochTime);
            break;
        }
    }
}

IOTHUBMESSAGE_DISPOSITION_RESULT receiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void *userContextCallback)
{
    IOTHUBMESSAGE_DISPOSITION_RESULT result;
    const unsigned char *buffer;
    size_t size;
    if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        Serial.println("Unable to IoTHubMessage_GetByteArray.");
        result = IOTHUBMESSAGE_REJECTED;
    }
    else
    {
        /*buffer is not zero terminated*/
        char *temp = (char *)malloc(size + 1);

        if (temp == NULL)
        {
            return IOTHUBMESSAGE_ABANDONED;
        }

        strncpy(temp, (const char *)buffer, size);
        temp[size] = '\0';
        Serial.printf("Receive C2D message: %s.\r\n", temp);
        char *temps = strstr(temp, "Temp"); 
        int pos = temps - temp; 
        String temp2;
        temp2 = String(temp[pos+14])+ String(temp[pos+14]);
        int temp_ = atoi(temp2.c_str());
        if (temp_ >= 25){
           blinkLED();
        }
        free(temp);
    }
    return IOTHUBMESSAGE_ACCEPTED;
}

static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

void setup() {
  Serial.begin(115200);
  initWiFi();
  initTime();
  pinMode(13, OUTPUT);
  iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (iotHubClientHandle == NULL)
    {
        Serial.println("Failed to connect to IoTHub.");
        while (1);
    }
    IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL);
}

static int messageCount = 1;

void loop() {
    //for only rcieving messages, the messageCallback function works in the background
    IoTHubClient_LL_DoWork(iotHubClientHandle);
    delay(10);
}
