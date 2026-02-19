/*
 * SmartPlant - Temperature & Soil Moisture Monitor
 * Simplified version for Arduino Uno R4
 * 
 * Sensors:
 * - DHT11 Temperature/Humidity Sensor on Digital Pin 8
 * - Soil Moisture Sensor on Analog Pin A0
 * 
 * Features:
 * - WiFi connection (ThingSpeak)
 * - Data logging to ThingSpeak
 */

#include <WiFiS3.h>             // WiFi library for Arduino Uno R4
#include "arduino_secrets.h"    // Credentials file
#include <DHT.h>                // Library for DHT11 sensor
#include <ThingSpeak.h>         // Library for ThingSpeak

// Pin Definitions
#define dht11SensorPin 8        // DHT11 on Digital Pin 8
#define moistSensorPin A0       // Soil Moisture on Analog Pin A0

// DHT11 Object
DHT dht11(dht11SensorPin, DHT11);

// WiFi Credentials from arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

// ThingSpeak Credentials
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
WiFiClient client;

// Global Variables
float humidity = 0.0;
float temperature = 0.0;
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

// Calibration values for soil moisture sensor
const int WaterValue = 1000;   // Sensor value in water
const int AirValue = 330;       // Sensor value in air

// Timing
unsigned long previousMillis = 0;
const long intervalThingSpeak = 900000;  // 15 minutes

void setup() {
    Serial.begin(9600);
    
    // Initialize sensors
    dht11.begin();
    pinMode(moistSensorPin, INPUT);
    
    // Connect to WiFi
    connect_to_wifi();
    
    // Initialize ThingSpeak
    ThingSpeak.begin(client);
    
    Serial.println("Setup complete - Starting monitoring...");
}

void loop() {
    // Read sensors
    read_sensors();
    
    // Send data to ThingSpeak every 15 minutes
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= intervalThingSpeak) {
        previousMillis = currentMillis;
        send_to_thing_speak();
    }
    
    // Debug output every 5 seconds
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Soil Moisture: ");
    Serial.print(soilMoisturePercent);
    Serial.println("%");
    
    delay(5000);  // 5 second delay between readings
}

void read_sensors() {
    // Read DHT11
    humidity = dht11.readHumidity();
    temperature = dht11.readTemperature();
    
    // Read Soil Moisture
    soilMoistureValue = analogRead(moistSensorPin);
    soilMoisturePercent = map(soilMoistureValue, WaterValue, AirValue, 0, 100);
    
    // Constrain percentage
    soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
}

void send_to_thing_speak() {
    // Check WiFi connection
    if (status != WL_CONNECTED) {
        connect_to_wifi();
    }
    
    // Set ThingSpeak fields
    // Field 1: Temperature
    // Field 2: Humidity  
    // Field 3: Soil Moisture (%)
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, soilMoisturePercent);
    
    // Write to ThingSpeak
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    if (x == 200) {
        Serial.println("ThingSpeak update successful");
    } else {
        Serial.print("ThingSpeak error: ");
        Serial.println(x);
    }
}

void connect_to_wifi() {
    // Check for WiFi module
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("WiFi module not found");
        while (true);
    }
    
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade WiFi firmware");
    }
    
    // Attempt connection
    while (status != WL_CONNECTED) {
        Serial.print("Connecting to WiFi: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000);
    }
    
    Serial.println("WiFi connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}
