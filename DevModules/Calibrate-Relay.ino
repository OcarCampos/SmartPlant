/*
Script for the calibration of a Relay Module
Last modified: 01/07/2025
Working with Arduino Uno R4 wifi
Sensors used: Relay module with external pump connected to external power source.

With this calibration script you should find out
if the relay module pin in low turns of the relay or not.
Depending on the result you should configure the script to
turn on and off the relay accordingly.

In my particular case and after testing, High is off and Low is on,
therefore in my production script I configured the following variables:

const byte relayOn = LOW;
const byte relayOff = HIGH;
*/

#define RELAY_PIN 2

// Setup of the script
void setup() {
  // initialize digital pin RELAY_PIN as an output.
  pinMode(RELAY_PIN, OUTPUT);   // Relay Pin Initialization 
  digitalWrite(RELAY_PIN, LOW); // Turn the Relay Pin Off by default (is this off? we find out now)
}

// Loop of the script
void loop() {
  digitalWrite(RELAY_PIN, HIGH);   // turn the RELAY pin on 
  delay(1000);                     // wait for a second
  digitalWrite(RELAY_PIN, LOW);    // turn the RELAY pin off
  delay(1000);                     // wait for a second
}