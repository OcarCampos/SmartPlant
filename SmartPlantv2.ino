/*
 * SmartPlant v2 - Arduino Nano Sensor Test Sketch
 * 
 * Hardware Setup:
 * - Arduino Nano V3 (CH340)
 * - TEMT6000 light sensor: VCC->5V, GND->GND, OUT->A0
 * - AHT10 temp/humidity: SDA->A4, SCL->A5, GND->GND, VIN->5V (I2C address: 0x38)
 * - Capacitive soil moisture: VCC->5V, GND->GND, AOUT->A1
 * 
 * This sketch reads all three sensors and prints values to Serial Monitor every 2 seconds
 * Baud rate: 9600
 */

// Required Libraries
#include <Wire.h>   
#include <Adafruit_AHTX0.h>

// Create AHT10 object
Adafruit_AHTX0 aht;

// Pin definitions
const int LIGHT_PIN = A0;
const int SOIL_PIN = A1;

// Struct to hold multiple values from AHT10
struct AHTReadings {
    float temperature;
    float humidity;
};

// Timing
unsigned long previousMillis = 0;
const long interval = 2000;  // 2 seconds

void setup() {

  // Initialize Serial communication
  Serial.begin(9600);
  Serial.println("[SmartPlant v2] Starting...");
  
  // Initialize AHT10 sensor
  if (aht.begin()) {
    Serial.println("[SmartPlant v2] AHT10 initialized successfully");
  } else {
    Serial.println("[SmartPlant v2] AHT10 initialization failed");
  }

  // Initialize light sensor
  pinMode(LIGHT_PIN, INPUT);
  Serial.println("[SmartPlant v2] Light sensor initialized successfully");

  // Initialize soil moisture sensor
  pinMode(SOIL_PIN, INPUT);
  Serial.println("[SmartPlant v2] Soil moisture sensor initialized successfully");

  // Final start up message
  Serial.println("[SmartPlant v2] System ready - starting sensor readings...");
  Serial.println();
}

void loop() {
  // Check if it's time to read the sensors
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read all sensors
    AHTReadings data = AHT10_READ(); 
    float lux = TEMT6000_READ();
    int soil = SOIL_MOISTURE_READ();
    
    // Horizontal data log
    Serial.print("[SmartPlant v2] Data -> ");
    Serial.print("Temp: "); Serial.print(data.temperature, 1); Serial.print("°C | ");
    Serial.print("Hum: ");  Serial.print(data.humidity, 1);    Serial.print("% | ");
    Serial.print("Light: "); Serial.print(lux, 0);             Serial.print(" lux | ");
    Serial.print("Soil Raw: "); Serial.println(soil);

  }
}

AHTReadings AHT10_READ(){
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);  // Read temperature and humidity

    // Create a readings object and populate it
    AHTReadings readings;
    readings.temperature = temp.temperature;
    readings.humidity = humidity.relative_humidity;

    return readings; // Return the whole object
}

float TEMT6000_READ(){
    int analogValue = analogRead(LIGHT_PIN);        // Read TEMT6000 analog value (0-1023)
    float voltage = analogValue * (5.0 / 1023.0);   // Convert to voltage (0-5V)
    // Convert voltage to approximate lux
    // TEMT6000 sensitivity is approximately 0.5 µA per lux
    // With 10k resistor: 1 lux ≈ 0.005 V
    // Max range: ~1000 lux (approximate)
    float lux = voltage * 200.0;  // Approximate conversion factor
    return lux;
}

int SOIL_MOISTURE_READ(){
    // Raw data from sensor
    int soilValue = analogRead(SOIL_PIN);
    return soilValue;
}

