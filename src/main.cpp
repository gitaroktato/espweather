#include <Arduino.h>

/*
 ESP8266 DHT11 --> ThingSpeak Channel
 
 This sketch sends the DHT11 temperature/humidity of an ESP8266 to a ThingSpeak
 channel using the ThingSpeak API (https://www.mathworks.com/help/thingspeak).
 
 Requirements:
 
   * ESP8266 Wi-Fi Device
   * Arduino 1.8.8+ IDE
   * kY-015 sensor board with DHT11
   * Additional Boards URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
   * Library: esp8266 by ESP8266 Community
   * Library: ThingSpeak by MathWorks
 
 ThingSpeak Setup:
 
   * Sign Up for New User Account - https://thingspeak.com/users/sign_up
   * Create a new Channel by selecting Channels, My Channels, and then New Channel
   * Enable one field
   * Enter SECRET_CH_ID in "secrets.h"
   * Enter SECRET_WRITE_APIKEY in "secrets.h"

 Setup Wi-Fi:
  * Enter SECRET_SSID in "secrets.h"
  * Enter SECRET_PASS in "secrets.h"
  
 Tutorial: http://nothans.com/measure-wi-fi-signal-levels-with-the-esp8266-and-thingspeak
   
 Created: Feb 1, 2017 by Hans Scharler (http://nothans.com)
*/


#include <DHT.h>
#include <ESP8266WiFi.h>

#include "ThingSpeak.h"
#include "secrets.h"

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;


char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key index number (needed only for WEP)
WiFiClient  client;

#define DHT_SENSOR_PIN  D7 // ESP8266 pin D7 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

#define SERIAL_BAUDRATE 9600

/**
 * Writes field to the specified ThingSpeak channel
*/
void writeFieldToThingSpeakChannel(unsigned int fieldId, float value) {
  int httpCode = ThingSpeak.writeField(myChannelNumber, fieldId, value, myWriteAPIKey);
  if (httpCode == 200) {
    Serial.println("Field #" + String(fieldId) + " write successful.");
  } else {
    Serial.println("Problem writing to field #" + String(fieldId) + " - HTTP error code " + String(httpCode));
  }
}

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  delay(100);

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);
  dht_sensor.begin(); // initialize the DHT sensor
}

void loop() {
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  // Measure Signal Strength (RSSI) of Wi-Fi connection
  // long rssi = WiFi.RSSI();

  // read humidity
  float humi  = dht_sensor.readHumidity();
  // read temperature in Celsius
  float tempC = dht_sensor.readTemperature();
  // read temperature in Fahrenheit
  float tempF = dht_sensor.readTemperature(true);

  // check whether the reading is successful or not
  if ( isnan(tempC) || isnan(tempF) || isnan(humi)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print("°C  ~  ");
    Serial.print(tempF);
    Serial.println("°F");
  }

  ThingSpeak.setField(1, tempC);
  ThingSpeak.setField(2, humi);
  
  int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (httpCode == 200) {
    Serial.println("Channel write successful.");
  } else {
    Serial.println("Problem writing to channel - HTTP error code " + String(httpCode));
  }

  // Wait 20 seconds to update the channel again
  delay(20000);
}

