/*
 * Arduino Program for SmartPlant v2
 * Working with Arduino Uno R4 wifi
 * Sensors used: AHT10, TEMT6000, Capacitive Soil Moisture
 * Connection to Wifi and ThingSpeak using secrets.h
 */

// Libraries
#include <WiFiS3.h>             // WiFi library for arduino uno
#include "arduino_secrets.h"    // Secrets file with credentials
#include <Wire.h>               // Library for I2C
#include <Adafruit_AHTX0.h>     // Library for AHT10 sensor.
#include <ThingSpeak.h>         // Library for ThingSpeak

// Pins used in the card by the sensors.
const int LIGHT_PIN = A0;    // Pin for Light Sensor (TEMT6000) - Analog Pin
const int SOIL_PIN = A1;     // Pin for Moisture Sensor - Analog Pin

/*
 * Objects
 */
Adafruit_AHTX0 aht;                     // AHT10 Object.

/*
 * Setting up the WiFi credentials.
 */
char ssid[] = SECRET_SSID;        // Network SSID (name) coming from secrests.h
char pass[] = SECRET_PASS;        // Network password (use for WPA, or use as key for WEP) coming from secrets.h
int status = WL_IDLE_STATUS;      // WiFi radio's status

/*
 * Setting up the ThingSpeak credentials.
 */
unsigned long myChannelNumber = SECRET_CH_ID;       // ThingSpeak Channel ID coming from secrets.h
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;   // ThingSpeak Write API Key coming from secrets.h
String myStatus = "";                               // ThingSpeak status for channel
WiFiClient client;                                  // WiFi client object for ThingSpeak connection

/*
 * Calibration values for the sensors
 */

/*
 * Soil Moisture Sensor:
 * ----------------------
*/
const int WaterValue = 210;        // According to calibration
const int AirValue = 498;           // According to calibration

/*
 * Global Variables
 */

// AHT10 Sensor
float humidity = 0.0;
float temperature = 0.0;

// Struct to hold multiple values from AHT10
struct AHTReadings {
    float temperature;
    float humidity;
};

// LDR sensor (TEMT6000)
float luxValue = 0.0;

// Soil Moisture Sensor
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

/*
 * Time Frames in ms.
 */
unsigned long previousMillis = 0;           // Stores last time the information was sent to thingspeak
const int intervalStart = 5000;             // 5 seconds of welcoming message
const int intervalPause = 3000;             // 3 seconds pause for clearing the screen
const int intervalThingSpeak = 900000;      // 15 minutes interval for reporting to ThingSpeak

/*
* Debug mode
*/
const bool DEBUG_MODE = false;            // Debug mode for printing values to serial monitor


/*
 * Setup of the program.
 */
void setup() {
    Serial.begin(9600);
    
    Serial.println("[SmartPlant v2] Welcome!");
    delay(intervalStart);

    // Connect to WiFi
    Serial.println("[SmartPlant v2] Connecting WiFi. Please wait...");
    connect_to_wifi();                  

    // Sensors Initialization
    Serial.println("[SmartPlant v2] Starting sensors. Please wait...");
    
    if (aht.begin()) {
        Serial.println("[SmartPlant v2] AHT10 initialized successfully");
    } else {
        Serial.println("[SmartPlant v2] AHT10 initialization failed");
    }

    pinMode(LIGHT_PIN, INPUT);          // Light Pin Initialization
    pinMode(SOIL_PIN, INPUT);           // Soil Pin Initialization
    ThingSpeak.begin(client);           // ThingSpeak Initialization
    delay(intervalStart);

    // Init done
    Serial.println("[SmartPlant v2] Setup done. Starting now...");
    delay(intervalStart);              // Delay before moving on to loop
    Serial.println("[SmartPlant v2] Reading sensors now...");
}

/*
 * Running loop.
 */
void loop() {
    // Check wifi connection. If not connected, try to reconnect.
    if (status != WL_CONNECTED) {
        Serial.println("[SmartPlant v2] Reconnecting WiFi. Please wait...");
        connect_to_wifi();                  
    }

    // Reading sensor values
    sensor_readings();    

    if (DEBUG_MODE){
        // Print to Serial for debugging
        Serial.print("[SmartPlant v2] Data -> ");
        Serial.print("Temp: "); Serial.print(temperature, 1); Serial.print("C | ");
        Serial.print("Hum: ");  Serial.print(humidity, 1);    Serial.print("% | ");
        Serial.print("Light: "); Serial.print(luxValue, 0);   Serial.print(" lux | ");
        Serial.print("Soil Raw: "); Serial.print(soilMoistureValue); Serial.print(" | ");
        Serial.print("Soil Moist: "); Serial.print(soilMoisturePercent); Serial.println("%");
    }

    // Sending sensor values to ThingSpeak
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= intervalThingSpeak) {
        previousMillis = currentMillis; 
        send_to_thing_speak();          
    }

    // End of loop
    delay(intervalPause);
}

AHTReadings AHT10_READ(){
    sensors_event_t hum, temp;
    aht.getEvent(&hum, &temp);  // Read temperature and humidity

    // Create a readings object and populate it
    AHTReadings readings;
    readings.temperature = temp.temperature;
    readings.humidity = hum.relative_humidity;

    return readings; // Return the whole object
}

float TEMT6000_READ(){
    int analogValue = analogRead(LIGHT_PIN);        // Read TEMT6000 analog value (0-1023)
    float voltage = analogValue * (5.0 / 1023.0);   // Convert to voltage (0-5V)
    float lux = voltage * 200.0;  // Approximate conversion factor
    return lux;
}

int SOIL_MOISTURE_READ(){
    // Raw data from sensor
    int soilValue = analogRead(SOIL_PIN);
    return soilValue;
}

/*
 * Function: sensor_readings()
 */
void sensor_readings() {
    // Reading AHT10 values
    AHTReadings data = AHT10_READ();
    humidity = data.humidity;        
    temperature = data.temperature;  

    // Reading Light value
    luxValue = TEMT6000_READ(); 

    // Reading Soil Moisture value
    soilMoistureValue = SOIL_MOISTURE_READ();
    // Calculate moisture percentage according to calibration values
    soilMoisturePercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);     
}

/* 
 * Function: send_to_thing_speak()
 */
void send_to_thing_speak() {
    // Set the fields with the values
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, luxValue);
    ThingSpeak.setField(4, soilMoisturePercent);
    ThingSpeak.setField(5, soilMoistureValue);

    // Set the status message
    myStatus = "[SmartPlant v2] Data updated successfully!";
    ThingSpeak.setStatus(myStatus);

    // Write to ThingSpeak channel.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    delay(1000); // Give some time for the connection to complete
    
    if (DEBUG_MODE){
        // Report result of connection to ThingSpeak
        if (x == 200 || x == 0) {
            Serial.println("[SmartPlant v2] ThingSpeak has been updated.");
        }
        else {
            Serial.println("[SmartPlant v2] ThingSpeak has NOT been updated.");
        }
        delay(intervalPause);
    }
}

/*
 * Function: connect_to_wifi()
 */
void connect_to_wifi() {
    // Check for the WiFi module
    if (WiFi.status() == WL_NO_MODULE) {
        // don't continue
        while (true);
    }
    // Check wifi firmware version
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("[SmartPlant v2] Please upgrade the firmware");
    }
    // Attempt to connect to WiFi network:
    while (status != WL_CONNECTED) {
        Serial.print("[SmartPlant v2] Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network:
        status = WiFi.begin(ssid, pass);
        // wait 10 seconds for connection:
        delay(10000);
    }
    // Connected now. Print out success message.
    Serial.println("[SmartPlant v2] WiFi Connected. Continuing...");
    delay(intervalStart);
}
