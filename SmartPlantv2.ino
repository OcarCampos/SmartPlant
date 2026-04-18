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

#include <Wire.h>

// Pin definitions
const int LIGHT_PIN = A0;
const int SOIL_PIN = A1;

// AHT10 I2C address
const int AHT10_ADDRESS = 0x38;

// Timing
unsigned long previousMillis = 0;
const long interval = 2000;  // 2 seconds

// AHT10 calibration data
float aht10Temperature = 0.0;
float aht10Humidity = 0.0;

void setup() {
  Serial.begin(9600);
  
  // Initialize I2C for AHT10
  Wire.begin();
  
  // Initialize AHT10 sensor
  if (initializeAHT10()) {
    Serial.println("[SmartPlant v2] AHT10 initialized successfully");
  } else {
    Serial.println("[SmartPlant v2] AHT10 initialization failed");
  }
  
  Serial.println("[SmartPlant v2] System ready - starting sensor readings...");
  Serial.println();
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read all sensors
    readAndPrintSensors();
  }
}

bool initializeAHT10() {
  // Send initialization command to AHT10
  // Command: 0xE1, 0x08, 0x00
  Wire.beginTransmission(AHT10_ADDRESS);
  Wire.write(0xE1);
  Wire.write(0x08);
  Wire.write(0x00);
  byte error = Wire.endTransmission();
  
  if (error != 0) {
    return false;
  }
  
  delay(10);  // Wait for initialization to complete
  return true;
}

bool readAHT10() {
  // Send measurement command to AHT10
  // Command: 0xAC, 0x33, 0x00
  Wire.beginTransmission(AHT10_ADDRESS);
  Wire.write(0xAC);
  Wire.write(0x33);
  Wire.write(0x00);
  byte error = Wire.endTransmission();
  
  if (error != 0) {
    return false;
  }
  
  delay(75);  // Wait for measurement to complete (AHT10 needs ~75ms)
  
  // Read 6 bytes of data
  Wire.requestFrom(AHT10_ADDRESS, 6);
  
  if (Wire.available() < 6) {
    return false;
  }
  
  // Read the data bytes
  byte statusByte = Wire.read();
  byte humHigh = Wire.read();
  byte humLow = Wire.read();
  byte tempHigh = Wire.read();
  byte tempLow = Wire.read();
  byte crc = Wire.read();
  
  // Check if measurement is valid (bit 7 of status byte should be 0)
  if (statusByte & 0x80) {
    return false;
  }
  
  // Calculate humidity
  // Humidity = ((humHigh << 8) | humLow) * 100.0 / 1048576.0
  uint32_t humRaw = ((uint32_t)humHigh << 8) | humLow;
  aht10Humidity = (humRaw * 100.0) / 1048576.0;
  
  // Calculate temperature
  // Temperature = ((tempHigh << 8) | tempLow) * 200.0 / 1048576.0 - 50.0
  uint32_t tempRaw = ((uint32_t)tempHigh << 8) | tempLow;
  aht10Temperature = (tempRaw * 200.0) / 1048576.0 - 50.0;
  
  return true;
}

float readLightSensor() {
  // Read TEMT6000 analog value (0-1023)
  int analogValue = analogRead(LIGHT_PIN);
  
  // Convert to voltage (0-5V)
  float voltage = analogValue * (5.0 / 1023.0);
  
  // Convert voltage to approximate lux
  // TEMT6000 sensitivity is approximately 0.5 µA per lux
  // With 10k resistor: 1 lux ≈ 0.005 V
  // Max range: ~1000 lux (approximate)
  float lux = voltage * 200.0;  // Approximate conversion factor
  
  return lux;
}

int readSoilMoisture() {
  // Read capacitive soil moisture sensor (raw analog value 0-1023)
  // Higher values = more moisture
  // Lower values = drier soil
  int soilValue = analogRead(SOIL_PIN);
  
  return soilValue;
}

void readAndPrintSensors() {
  // Read light sensor
  float lightLux = readLightSensor();
  
  // Read AHT10
  bool ahtSuccess = readAHT10();
  
  // Read soil moisture
  int soilRaw = readSoilMoisture();
  
  // Print formatted output
  Serial.print("[SmartPlant v2] --- Light: ");
  Serial.print(lightLux, 1);
  Serial.print(" lux | Temp: ");
  
  if (ahtSuccess) {
    Serial.print(aht10Temperature, 1);
    Serial.print(" °C | Humidity: ");
    Serial.print(aht10Humidity, 1);
    Serial.print(" %");
  } else {
    Serial.print("-- C | Humidity: -- %");
  }
  
  Serial.print(" | Soil Raw: ");
  Serial.print(soilRaw);
  Serial.println(" ---");
}