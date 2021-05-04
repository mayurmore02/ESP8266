/* EnerGient Solutions */
// code for simply interfacing dht11 with esp8266 and showing data over internet using MQTT protocol and Adafruit IO website

//Include library for ESP8266 configuration   

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"                      //Include library for MQTT 
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"                                //Include library for Temperature and humidity sensor configuration

// DHT11 or DHT22 pin and Type declaration

#define DHTPIN 12                                //DHT11 ouptput connected to ESP-01
#define DHTTYPE DHT11                           //DHT TYPE-DHT 11 or 22

DHT dht(DHTPIN, DHTTYPE, 15);                   //Create the instance for DHT sensor

// WiFi parameters

#define WLAN_SSID       "OPPO F9"                 //SSID of Home's WiFi network
#define WLAN_PASS       "12345678"             //Password of Home's WiFi network

// Adafruit IO Broker Initialization

#define AIO_SERVER      "io.adafruit.com"      //Broker domain Name
#define AIO_SERVERPORT  1883                   //The port at which broker is listening
#define AIO_USERNAME    "mayurmore02"              //The username which we have initializaed during account creation at io.adafruit.com
#define AIO_KEY         "aio_Qaoa29tlfDxicGA2vgoo7ZHfYsFU"  //The private ley which will be alloted after creating account

//Create an ESP8266 WiFiClient class to connect to the MQTT server.

WiFiClient espclient;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.

Adafruit_MQTT_Client mqtt(&espclient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

//Create the topic to be registered on MQTT broker

//Notice MQTT paths for AIO follow the form: /feeds/

Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");

//Function declaration

void MQTT_connect();

void setup() {

  Serial.begin(115200);                   //Start the serial communication for debugging 
  delay(10);
  dht.begin();                           //DHT sensor initialization

  //Connect to WiFi access point.

  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  // Wait until the connection has been confirmed before continuing

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
      Serial.print(".");
    }

  Serial.println();
  Serial.println("WiFi connected");
    Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

}

void loop() {

  MQTT_connect();
 // float temperature_data = 32.0; //dht.readTemperature();     //Read the current temperature
  float temperature_data = dht.readTemperature();
 // float humidity_data = 51.0; //dht.readHumidity();           //Read the current humidity
  float humidity_data = dht.readHumidity();

  // Now we can publish stuff!

  Serial.print(F("nSending temperature value "));
  Serial.print(temperature_data);
  Serial.print("...");

  //Publish the temperature to the Adafruit broker

  if (! temp.publish(temperature_data)) 
  {
    Serial.println(F("Failed"));
    } else 
    {
      Serial.println(F("OK!"));
      }


    Serial.print(F("nSending Humidity value "));
    Serial.print(humidity_data);
    Serial.print("...");

    //Publish the humidity to the Adafruit broker

    if (! humidity.publish(humidity_data)) 
    {
      Serial.println(F("Failed"));
      } else 
    {
      Serial.println(F("OK!"));
      }
      delay(10000);
}

/*

Function Name- MQTT_connect()

Function to connect and reconnect as necessary to the MQTT server.

It should be called in the loop function and it will take care if connecting.

*/

 void MQTT_connect() {
  int8_t ret;

  //Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
    while ((ret = mqtt.connect()) != 0) {     // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);               // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  Serial.println("MQTT Connected!");
}
