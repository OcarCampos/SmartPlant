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
  // Reset the AHT10
  Wire.beginTransmission(AHT10_ADDRESS);
  Wire.write(0xBA);  // AHT10 soft reset command
  byte error = Wire.endTransmission();
  if (error != 0) {
    Serial.println("[AHT10] Reset: no response");
    return false;
  }
  delay(20);  // Wait for reset to complete (AHT10 needs ~5ms)

  // Send initialization command (AHT10 = 0xE1, AHT20 = 0xBE)
  Wire.beginTransmission(AHT10_ADDRESS);
  Wire.write(0xE1);
  Wire.write(0x08);
  Wire.write(0x00);
  error = Wire.endTransmission();
  if (error != 0) {
    Serial.println("[AHT10] Init: no response");
    return false;
  }
  delay(10);

  return true;
}

bool readAHT10() {
  // Request a measurement: 0xAC command (trigger reading, no hold)
  Wire.beginTransmission(AHT10_ADDRESS);
  Wire.write(0xAC);
  Wire.write(0x33);
  Wire.write(0x00);
  byte error = Wire.endTransmission();
  if (error != 0) return false;

  delay(80);  // AHT10 needs ~80ms for measurement

  // Read 6 bytes: status + 3 data bytes + 1 status/CRC
  Wire.requestFrom(AHT10_ADDRESS, (uint8_t)6);
  if (Wire.available() < 6) return false;

  byte statusByte = Wire.read();
  byte data1 = Wire.read();  // bits [19:12] of humidity
  byte data2 = Wire.read();  // bits [11:4] of humidity, bits [19:16] of temp
  byte data3 = Wire.read();  // bits [15:8] of temperature
  byte data4 = Wire.read();  // bits [7:0] of temperature (partial status/CRC)
  byte crc = Wire.read();    // CRC-8

  // Check busy flag (bit 7 of status — should be 0 when data is ready)
  if (statusByte & 0x80) {
    // Sensor still busy — skip this reading
    return false;
  }

  // Extract 20-bit humidity from bits [19:0] of the combined 3-byte field
  // layout: [hum[19:12] | hum[11:4] | temp[19:16] | temp[15:8] | temp[7:0]]
  // hum bits occupy: data1 (8 bits) + upper 4 bits of data2 (4 bits)
  uint32_t humRaw = (((uint32_t)data1) << 8) | ((uint32_t)data2 & 0xF0);
  humRaw = humRaw >> 4;  // shift right to get proper 20-bit value

  // Extract 20-bit temperature from bits [19:0]
  // temp bits occupy: lower 4 bits of data2 + data3 (8 bits) + upper 4 bits of data4 (4 bits)
  uint32_t tempRaw = (((uint32_t)data2 & 0x0F) << 16) | (((uint32_t)data3) << 8) | ((uint32_t)data4 & 0xF0);
  tempRaw = tempRaw >> 4;  // shift right to get proper 20-bit value

  // Convert to physical units
  // Humidity: 0..100 %RH  (20-bit max = 1048576)
  aht10Humidity = ((float)humRaw / 1048576.0) * 100.0;

  // Temperature: -40..125 °C  (20-bit max = 1048576, range = 165°C)
  aht10Temperature = (((float)tempRaw / 1048576.0) * 165.0) - 45.0;

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