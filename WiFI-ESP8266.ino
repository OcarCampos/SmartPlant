#include <SoftwareSerial.h>

// Connect ESP TX to 2, RX to 3
SoftwareSerial espSerial(2, 3); 

// Track connection state for visualization
bool isConnected = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Start with LED off

  Serial.begin(9600); // USB Serial
  espSerial.begin(9600); // ESP8266 Default baud rate

  Serial.println("Initializing...");
  delay(1000);

  // Check if ESP8266 is alive and responding
  Serial.println("Checking ESP8266 connection...");
  String aliveResp = sendAT("AT", 2000);
  
  if (aliveResp.indexOf("OK") == -1) {
    Serial.println("❌ ESP8266 not responding. Is it powered on and wired correctly?");
    // We can hang here or continue, let's just let the loop handle the error state
    isConnected = false;
    return; // Fast fail, skip the rest of setup
  }
  
  Serial.println("✅ ESP8266 is responding.");

  // Set WiFi mode to Client
  sendAT("AT+CWMODE=1", 2000);
  
  // Connect to Network
  Serial.println("Attempting to connect to WiFi...");
  String connectCmd = "AT+CWJAP=\"Jacinto\",\"lycalopex590\"";
  String response = sendAT(connectCmd, 15000); // Increased timeout to 15s to allow for connection

  Serial.println("\n-----------------------------------");
  // Check the response for success indicators
  if (response.indexOf("WIFI GOT IP") != -1 || response.indexOf("OK") != -1) {
    Serial.println("✅ WiFi Connected Successfully!");
    isConnected = true;
    digitalWrite(LED_BUILTIN, HIGH); // Solid LED means Success
  } else {
    Serial.println("❌ WiFi Connection Failed.");
    isConnected = false;
  }
  Serial.println("-----------------------------------\n");
}

void loop() {
  if (!isConnected) {
    // Blink LED to visualize failure state
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }

  // Pass data from ESP to Serial monitor
  if (espSerial.available()) {
    Serial.write(espSerial.read());
  }
  
  // Allow manual AT commands from Serial Monitor to ESP
  if (Serial.available()) {
    espSerial.write(Serial.read());
  }
}

String sendAT(String command, const int timeout) {
  String response = "";
  Serial.println("Sending: " + command);
  espSerial.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (espSerial.available()) {
      char c = espSerial.read();
      response += c;
      Serial.write(c); // Print response to Serial Monitor
    }
  }
  return response;
}
