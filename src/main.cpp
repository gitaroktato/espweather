#include <Arduino.h>

/*
 ESP8266 BME280 --> ThingSpeak Channel
 
 This sketch sends the BME280 temperature/humidity of an ESP8266 to a ThingSpeak
 channel using the ThingSpeak API (https://www.mathworks.com/help/thingspeak).
 
 Requirements:
 
   * ESP8266 Wi-Fi Device
   * Arduino 1.8.8+ IDE
   * BME280 sensor board
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

#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>

#include "ThingSpeak.h"
#include "secrets.h"

#define SERIAL_BAUDRATE 9600
#define ONBOARD_LED 16
#define ESP8266_LED 2

#define SEALEVELPRESSURE_HPA (1013.25)

#define PHOTORESISTOR_PIN A0
// Report to ThingSpeak or not
#define REPORT_ON
// Use onbard LEDs to display current state or not
// #define LED_STATE_ON

#ifdef REPORT_ON
// Period in microseconds for channel update. Should be around 10-15 minutes
// #define PERIOD 15 * 60 * 1000 * 1000
#define PERIOD 15 * 1000 * 1000
//We will use static ip
IPAddress ip(192, 168, 2, 35);
IPAddress dns(192, 168, 1, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
#else
// Period in microseconds for troubleshooting. Should be around 15 seconds
#define PERIOD 15 * 1000 * 1000
#endif

// ThingSpeak channel and API key
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key index number (needed only for WEP)
WiFiClient  client;

Adafruit_BME280 bme; // I2C

bool ledState = 0x0;

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
  // Can't use with deep sleep
  // pinMode(ONBOARD_LED, OUTPUT);
  pinMode(ESP8266_LED, OUTPUT);

  Serial.begin(SERIAL_BAUDRATE);
  delay(100);

#ifdef REPORT_ON
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  ThingSpeak.begin(client);
#else
  WiFi.mode(WIFI_OFF);
#endif

  bool bme_status;
	bme_status = bme.begin(0x76);		//Begin the sensor
  if (!bme_status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {

  #ifdef REPORT_ON
    // Connect or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect to SSID: ");
      int attempts = 0;
      Serial.println(SECRET_SSID);
      // Turn on all LEDs
      // Can't use with deep sleep
      // digitalWrite(ONBOARD_LED, HIGH);
      digitalWrite(ESP8266_LED, HIGH);
      
      while (WiFi.status() != WL_CONNECTED) {
        WiFi.config(ip, dns, gateway, subnet);
        WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
        Serial.print(".");
        // Switch one LED
        ledState = !ledState;
        digitalWrite(ESP8266_LED, ledState);
        // Check number of attempts
        attempts++;
        if (attempts > 3) {
          Serial.println("No more connection attempts, reset.");
          return;
        }
        delay(5000);
      }
      Serial.println("\nConnected.");
    }
  #endif

  // Turn off all LEDs
  // Can't use with deep sleep
  // digitalWrite(ONBOARD_LED, LOW);
  digitalWrite(ESP8266_LED, LOW);

  // Measure Signal Strength (RSSI) of Wi-Fi connection
  #ifdef REPORT_ON
    long rssi = WiFi.RSSI();
    Serial.println("RSSI: " + String(rssi) + " dB");
  #endif
  // Getting Vcc from board
  float Vcc = (float)ESP.getVcc() / 65535 * 100;
  Serial.println("Vcc: " + String(Vcc) + "%");

  // BE280 read humidity
  float humi  = bme.readHumidity();
  // BE280 read temperature in Celsius
  float tempC = bme.readTemperature();
  // BMP280 reads
  float airPressure = bme.readPressure() / 100.0F;
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  // read photoresistor value
  float lux = analogRead(PHOTORESISTOR_PIN);

  // check whether the reading is successful or not
  if ( isnan(tempC) ||  isnan(humi)) {
    Serial.println("Failed to read from BME280 sensor!");
  } else {
    Serial.print("Humidity: " + String(humi) + "%");
    Serial.print("  |  ");
    Serial.println("Temperature: " + String(tempC) + "°C");
  }

  // check whether the reading is successful or not
  if ( isnan(airPressure) ||  isnan(altitude)) {
    Serial.println("Failed to read from BME280 sensor!");
  } else {
    Serial.print("Air pressure: " + String(airPressure) + " hPa"); 
    Serial.print("  |  ");
    Serial.println("Altitude: " + String(altitude) + " m");
  }

  if ( isnan(lux) ) {
    Serial.println("Failed to read from photoresistor!");
  } else {
    Serial.println(String(lux) + " lux");
  }

  #ifdef REPORT_ON
    ThingSpeak.setField(1, tempC);
    ThingSpeak.setField(2, humi);
    ThingSpeak.setField(3, lux);
    ThingSpeak.setField(4, Vcc);
    ThingSpeak.setField(5, rssi);
    ThingSpeak.setField(6, airPressure);
    ThingSpeak.setField(7, altitude);
    
    int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (httpCode == 200) {
      Serial.println("Channel write successful.");
    } else {
      Serial.println("Problem writing to channel - HTTP error code " + String(httpCode));
    }
  #endif

  #ifdef LED_STATE_ON
    // Switch two onboard LEDs
    ledState = !ledState;
    digitalWrite(ONBOARD_LED, ledState);
    digitalWrite(ESP8266_LED, !ledState);
  #endif

  // Wait to update the channel again
  Serial.println("Now on deep sleep for " + String(PERIOD / (1000 * 1000)) + " sec");
  // Normal sleep in milliseconds
  // delay(5 * 1000);
  // Deep sleep in microseconds
  ESP.deepSleep(PERIOD);
}
