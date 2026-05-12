#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <WiFi.h>
#include "ThingSpeak.h"

// PIN DEFINITIONS
#define LED_GREEN 4
#define LED_YELLOW 5
#define LED_RED 6
#define TEMT_PIN 0 // GPIO 0 (ADC1)
#define I2C_SDA 8
#define I2C_SCL 9

#include "arduino_secrets.h"

// CONSTANTS
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
unsigned long myChannelNumber = SECRET_CH_ID;
const char* myWriteAPIKey = SECRET_WRITE_APIKEY;

// DEBUG MODE
const bool DEBUG = true;

// Deep Sleep Interval (15 minutes in microseconds)
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 900 

Adafruit_AHTX0 aht;
WiFiClient client;

// Sinking Logic Helpers
void ledOn(int pin) { digitalWrite(pin, LOW); }
void ledOff(int pin) { digitalWrite(pin, HIGH); }

void errorAlert(const char* errorMsg) {
  if (DEBUG) {
    Serial.print("[SmartPlant v2] ERROR: ");
    Serial.println(errorMsg);
    Serial.println("[SmartPlant v2] Sleeping for 5 minutes before retry...");
    Serial.flush();
  }
  
  unsigned long start = millis();
  while (millis() - start < 120000) { // 2 Minute Visual Alert
    ledOn(LED_RED); delay(100); ledOff(LED_RED); delay(100); // Rapid blink
  }
  // Short sleep on error to try again soon (5 minutes)
  esp_sleep_enable_timer_wakeup(300 * uS_TO_S_FACTOR); 
  esp_deep_sleep_start();
}

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
    delay(1000); // Give Serial monitor time to connect
    Serial.println("\n[SmartPlant v2] Starting ESP32-C3...");
  }

  // Initialize Pins
  pinMode(TEMT_PIN, INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  // Ensure LEDs are OFF at boot (Sinking = HIGH is OFF)
  ledOff(LED_GREEN); ledOff(LED_YELLOW); ledOff(LED_RED);

  Wire.begin(I2C_SDA, I2C_SCL);
  ThingSpeak.begin(client);

  // 1. SELF-CHECK
  if (!aht.begin()) {
    errorAlert("AHT10 initialization failed");
  }
  if (DEBUG) Serial.println("[SmartPlant v2] AHT10 initialized successfully");

  // 2. READ SENSORS
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  int lightRaw = analogRead(TEMT_PIN);
  float lightVoltage = analogReadMilliVolts(TEMT_PIN) / 1000.0; // Use hardware calibrated millivolts
  float lightLevel = lightVoltage * 200.0;  // Same conversion factor as UNO version

  if (DEBUG) {
    Serial.print("[SmartPlant v2] Data -> Temp: "); Serial.print(temp.temperature);
    Serial.print("°C | Hum: "); Serial.print(humidity.relative_humidity);
    Serial.print("% | Light: "); Serial.print(lightLevel);
    Serial.print(" lux (raw: "); Serial.print(lightRaw); Serial.println(")");
  }

  // 3. WIFI CONNECTION
  if (DEBUG) {
    Serial.print("[SmartPlant v2] Connecting to WiFi: ");
    Serial.println(ssid);
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  WiFi.begin(ssid, password);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);  // Fix to broken antenna design.
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 60) { // 30 sec timeout
    // Heartbeat: blink yellow while connecting
    ledOn(LED_YELLOW);
    delay(250);
    ledOff(LED_YELLOW);
    delay(250);
    if (DEBUG) Serial.print(".");
    retryCount++;
  }

  if (DEBUG) Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    errorAlert("WiFi connection failed");
  }

  if (DEBUG) {
    Serial.println("[SmartPlant v2] WiFi connected successfully");
    Serial.print("[SmartPlant v2] IP Address: ");
    Serial.println(WiFi.localIP());
  }

  // 4. SEND TO THINGSPEAK
  if (DEBUG) Serial.println("[SmartPlant v2] Sending data to ThingSpeak...");
  ThingSpeak.setField(1, temp.temperature);
  ThingSpeak.setField(2, humidity.relative_humidity);
  ThingSpeak.setField(3, lightLevel);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  if (x == 200) {
    if (DEBUG) Serial.println("[SmartPlant v2] Data sent successfully to ThingSpeak");
    ledOn(LED_GREEN);
    delay(2000); // Visual confirmation
    ledOff(LED_GREEN);
  } else {
    String errorStr = "ThingSpeak write failed with code: " + String(x);
    errorAlert(errorStr.c_str());
  }

  // 5. DEEP SLEEP
  if (DEBUG) {
    Serial.print("[SmartPlant v2] Entering deep sleep for ");
    Serial.print(TIME_TO_SLEEP / 60);
    Serial.println(" minutes...");
    Serial.flush();
  }
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {
  // Loop is empty due to Deep Sleep
}