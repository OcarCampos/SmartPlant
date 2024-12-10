/*
Arduino Program to test connection to ThingSpeak via WiFi
using the Arduino UNO R4 board.

It reads temperature and humidity from a DHT11 sensor and
logs the information via wifi to ThingSpeak.

It requires the arduino_secrets.h file with the credentials
for the wifi network and for the ThingSpeak portal.

The arduino IDE requires to install Install ThingSpeak Communication 
Library for Arduino

In the Arduino IDE, choose Sketch/Include Library/Manage Libraries. 
Click the ThingSpeak Library from the list, and click 
the Install button.

Last modified: 12/10/2024
*/

#include <WiFiS3.h>             // WiFi library for arduino uno
#include "arduino_secrets.h"    // Secrets file with credentials
#include <DHT.h>                //Library for DHT11 sensor.
#include "ThingSpeak.h"         //Library for ThingSpeak

#define dht11SensorPin 8        //Pin for DHT11 - Digital Pin

DHT dht11(dht11SensorPin, DHT11);       //Object for DHT11

// For WiFi
char ssid[] = SECRET_SSID;      // network SSID
char pass[] = SECRET_PASS;      // network password (use for WPA)
int status = WL_IDLE_STATUS;    // the WiFi radio's status

// For ThingSpeak
unsigned long myChannelNumber = SECRET_CH_ID;       // ThingSpeak Channel ID
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;   // ThingSpeak Write API Key
String myStatus = "";                               // ThingSpeak status for channel

// Variables for DHT11 Sensor
float humidity = 0.0;
float temperature = 0.0;

// Time interval variables to report to Thingspeak
const long interval = 60000; // 60 seconds interval
unsigned long previousMillis = 0;  // Will store last time information was sent to thingspeak

// For future references
// 5 minutes are 300000 milliseconds
// 10 minutes are 600000 milliseconds
// 15 minutes are 900000 milliseconds
// 20 minutes are 1200000 milliseconds
// 25 minutes are 1500000 milliseconds
// 30 minutes are 1800000 milliseconds
// 35 minutes are 2100000 milliseconds
// 40 minutes are 2400000 milliseconds
// 45 minutes are 2700000 milliseconds
// 50 minutes are 3000000 milliseconds
// 55 minutes are 3300000 milliseconds
// 60 minutes are 3600000 milliseconds

/*
 * Setting up the variables to run in the card.
 */
void setup() {
    Serial.begin(9600);
    
    connect_to_wifi();              // Try to connect to the wifi given the credentials

    dht11.begin();                  // DHT11 Initialization
    ThingSpeak.begin(client);       // ThingSpeak Initialization
    
}

/*
 * Running code and looping over the DHT11 sensor.
 */
void loop() {
    Serial.println("Looping over DHT11 sensor...");
    sensor_readings();              // Read sensor values
    serial_monitor_printing();      // Print sensor values
    unsigned long currentMillis = millis(); // Get current time
    // If passed time is greater than interval
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; // Update previousMillis
        Serial.println("Sending data to ThingSpeak...");
        send_to_thing_speak();          // Send sensor values to ThingSpeak
    }
    delay(1000); //delay 1 second to read temperature and humidity again
}

/* 
 * Function that reads the sensors values
 */
void sensor_readings() {
    // Reading DHT11 values
    humidity = dht11.readHumidity();        // Read humidity
    temperature = dht11.readTemperature();  // Read temperature
}

/* 
 * Function that prints the sensor values
 */
void serial_monitor_printing() {
    // Printing to Serial Monitor
    Serial.print("Air Humidity: ");
    Serial.print(humidity);
    Serial.println(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");
}

/* 
 * Function that sends the sensor values to ThingSpeak
 */
void send_to_thing_speak() {
    // Set the fields with the values
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);

    // Set the status message
    myStatus = "Data updated successfully.";
    ThingSpeak.setStatus(myStatus);

    // Write to ThingSpeak channel.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
        Serial.println("Channel update successful.");
    }
    else {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
}

/*
 * Function that connects to the wifi given the credentials
 */
void connect_to_wifi() {
    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
    }
    // check wifi firmware version
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
    }
    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network:
        status = WiFi.begin(ssid, pass);
        // wait 10 seconds for connection:
        delay(10000);
    }
    // you're connected now, so print out the data:
    Serial.println("You're connected to the network");
    printCurrentNet();
    printWifiData();
}

/*
 * Function that prints the wifi data
 */
void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

/*
 * Function that prints the current network to which the board is connected.
 */
void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
}