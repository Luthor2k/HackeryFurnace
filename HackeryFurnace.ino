/*
 *  Monitor the five thermostats at The Hackery for their state, send as mqtt to the dashboard
 *
 */

//#include <WiFi.h>
//#include <WiFiMulti.h>

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//WiFiMulti WiFiMulti;


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

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// or... use WiFiClientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish hackery_thermostats = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/heating/thermostats");

// Setup a feed called 'onoff' for subscribing to changes.
// Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/heating/onoff");


/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup()
{
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
  
  Serial.begin(115200);
  delay(5);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1500);

  // Setup MQTT subscription for onoff feed.
  //mqtt.subscribe(&onoffbutton);

}

uint32_t x=0;

void loop()
{
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  /*
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
    }
  }
  */

  zone1Status = !(digitalRead(zone1Pin));
  zone2Status = !(digitalRead(zone2Pin));
  zone3Status = !(digitalRead(zone3Pin));
  zone4Status = !(digitalRead(zone4Pin));
  zone5Status = !(digitalRead(zone5Pin));
  
  // Now we can publish stuff!
  Serial.print(F("\nSending hackery_thermostats val "));
  Serial.print(x);
  Serial.print("...");
  if (! hackery_thermostats.publish(x++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */

  Serial.println("Waiting 5 seconds before next message...");
  delay(5000);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
