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

// CONSTANTS
const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";
unsigned long myChannelNumber = YOUR_CHANNEL_ID;
const char* myWriteAPIKey = "YOUR API KEY";

// Deep Sleep Interval (15 minutes in microseconds)
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 900 

Adafruit_AHTX0 aht;
WiFiClient client;

// Sinking Logic Helpers
void ledOn(int pin) { digitalWrite(pin, LOW); }
void ledOff(int pin) { digitalWrite(pin, HIGH); }

void errorAlert() {
  unsigned long start = millis();
  while (millis() - start < 120000) { // 2 Minute Visual Alert
    ledOn(LED_RED); delay(100); ledOff(LED_RED); delay(100); // Rapid blink
  }
  // Short sleep on error to try again soon (5 minutes)
  esp_sleep_enable_timer_wakeup(300 * uS_TO_S_FACTOR); 
  esp_deep_sleep_start();
}

void setup() {
  // Initialize Pins
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  // Ensure LEDs are OFF at boot (Sinking = HIGH is OFF)
  ledOff(LED_GREEN); ledOff(LED_YELLOW); ledOff(LED_RED);

  Wire.begin(I2C_SDA, I2C_SCL);
  ThingSpeak.begin(client);

  // 1. SELF-CHECK
  if (!aht.begin()) {
    errorAlert();
  }

  // 2. READ SENSORS
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  int lightLevel = analogRead(TEMT_PIN);

  // 3. WIFI CONNECTION
  WiFi.begin(ssid, password);
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 60) { // 30 sec timeout
    // Heartbeat: blink yellow while connecting
    ledOn(LED_YELLOW);
    delay(250);
    ledOff(LED_YELLOW);
    delay(250);
    retryCount++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    errorAlert();
  }

  // 4. SEND TO THINGSPEAK
  ThingSpeak.setField(1, temp.temperature);
  ThingSpeak.setField(2, humidity.relative_humidity);
  ThingSpeak.setField(3, lightLevel);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  if (x == 200) {
    ledOn(LED_GREEN);
    delay(2000); // Visual confirmation
    ledOff(LED_GREEN);
  } else {
    errorAlert();
  }

  // 5. DEEP SLEEP
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {
  // Loop is empty due to Deep Sleep
}