#include "ThingSpeak.h"
#include "secrets.h"
#include <ESP8266WiFi.h>

int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = API_KEY;

const int zone1Pin = 4;   //storefront
const int zone2Pin = 5;   //main workshop
const int zone3Pin = 14;  //upstairsA
const int zone4Pin = 12;  //North room
const int zone5Pin = 16;  //upstairsB

int zone1Status = 0;
int zone2Status = 0;
int zone3Status = 0;
int zone4Status = 0;
int zone5Status = 0;

void setup() {
  Serial.begin(9600);  // Initialize serial

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  pinMode(zone1Pin, INPUT); 
  pinMode(zone2Pin, INPUT); 
  pinMode(zone3Pin, INPUT); 
  pinMode(zone4Pin, INPUT); 
  pinMode(zone5Pin, INPUT); 

  digitalWrite(zone1Pin, LOW);  //disable all pullups
  digitalWrite(zone2Pin, LOW);
  digitalWrite(zone3Pin, LOW);
  digitalWrite(zone4Pin, LOW);
  digitalWrite(zone5Pin, LOW);
}

void loop() {

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  zone1Status = !(digitalRead(zone1Pin));
  zone2Status = !(digitalRead(zone2Pin));
  zone3Status = !(digitalRead(zone3Pin));
  zone4Status = !(digitalRead(zone4Pin));
  zone5Status = !(digitalRead(zone5Pin));

  // set the fields with the values
  ThingSpeak.setField(1, zone1Status * 255);
  ThingSpeak.setField(2, zone2Status * 255);
  ThingSpeak.setField(3, zone3Status * 255);
  ThingSpeak.setField(4, zone4Status * 255);
  ThingSpeak.setField(5, zone5Status * 255);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(60000); // Wait 60 seconds to update the channel again
}
