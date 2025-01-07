#define RELAY_PIN 7

void setup() {
  // initialize digital pin RELAY_PIN as an output.
  pinMode(RELAY_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(RELAY_PIN, HIGH);   // turn the RELAY pin on 
  delay(1000);                     // wait for a second
  digitalWrite(RELAY_PIN, LOW);    // turn the RELAY pin off
  delay(1000);                     // wait for a second
}