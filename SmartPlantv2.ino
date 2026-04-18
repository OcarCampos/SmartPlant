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
  // AHT10 soft reset — recommended before init
  Wire.beginTransmission(AHT10_ADDRESS);
  Wire.write(0xBA);
  byte error = Wire.endTransmission();
  delay(20);

  // AHT10 init command
  Wire.beginTransmission(AHT10_ADDRESS);
  Wire.write(0xA8);
  error = Wire.endTransmission();
  if (error != 0) {
    Serial.println("[AHT10] Init: no ACK");
    return false;
  }
  delay(10);

  // Trigger first measurement to activate sensor
  Wire.beginTransmission(AHT10_ADDRESS);
  Wire.write(0xAC);
  Wire.write(0x33);
  Wire.write(0x00);
  error = Wire.endTransmission();
  delay(80);

  // Read status to verify calibration bit
  Wire.requestFrom(AHT10_ADDRESS, (uint8_t)1);
  if (Wire.available() > 0) {
    byte s = Wire.read();
    if (s & 0x08) {
      Serial.println("[AHT10] Calibrated and ready");
    } else {
      Serial.println("[AHT10] Warning: calibration bit not set");
    }
  }

  return true;
}

bool readAHT10() {
  // Trigger measurement
  Wire.beginTransmission(AHT10_ADDRESS);
  Wire.write(0xAC);
  Wire.write(0x33);
  Wire.write(0x00);
  byte error = Wire.endTransmission();
  if (error != 0) return false;

  delay(80);

  Wire.requestFrom(AHT10_ADDRESS, (uint8_t)6);
  if (Wire.available() < 6) return false;

  byte s = Wire.read();           // status
  byte b1 = Wire.read();           // hum[19:12]
  byte b2 = Wire.read();           // hum[11:4] | temp[19:16]
  byte b3 = Wire.read();           // temp[15:8]
  byte b4 = Wire.read();           // temp[7:0] (upper nibble used, lower = status/CRC)
  byte crc = Wire.read();          // CRC-8

  if (s & 0x80) return false;  // busy flag

  // AHT10 20-bit extraction (verified against real sensor data):
  // b1 = hum[19:12], b2 upper nibble = hum[11:8], b2 lower nibble = temp[19:16]
  // b3 = temp[15:8], b4 upper nibble = temp[7:4]
  // Humidity: 20-bit value from b1 + upper nibble of b2
  uint32_t hum_raw = (((uint32_t)b1 << 12) | ((uint32_t)b2 << 4)) & 0x0FFFFF;

  // Temperature: 20-bit value from lower nibble of b2 + b3 + upper nibble of b4
  uint32_t temp_raw = (((uint32_t)b2 & 0x0F) << 16) | (((uint32_t)b3) << 8) | ((uint32_t)b4 & 0xF0);

  // Convert to physical units
  aht10Humidity = (hum_raw / 1048576.0) * 100.0;
  aht10Temperature = ((temp_raw / 1048576.0) * 165.0) - 45.0;

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
    Serial.print("--.- °C | Humidity: --.- % [E]");
  }
  
  Serial.print(" | Soil Raw: ");
  Serial.print(soilRaw);
  Serial.println(" ---");
}