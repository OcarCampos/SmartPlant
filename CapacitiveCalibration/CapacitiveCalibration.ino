/*
 * Capacitive Soil Moisture Sensor Calibration
 * Compatible with Arduino UNO
 *
 * Instructions:
 * 1. Connect your sensor to the board (Analog Pin A1).
 * 2. Upload this sketch to your Arduino UNO.
 * 3. Open the Serial Monitor (Tools -> Serial Monitor) at 9600 baud.
 * 4. For AirValue (Dry): Leave the sensor in dry air and note the value.
 * 5. For WaterValue (Wet): Submerge the sensor in water up to the safe line and note the value.
 * 6. Update the main script (SmartPlantv2-uno.ino) with these two values.
 */

const int SOIL_PIN = A1;  // Pin connected to the soil moisture sensor

void setup() {
  Serial.begin(9600);
  pinMode(SOIL_PIN, INPUT);
  
  Serial.println("=========================================");
  Serial.println(" Capacitive Sensor Calibration Script");
  Serial.println("=========================================");
  Serial.println("Reading values...");
  delay(2000);
}

void loop() {
  // Read the analog value from the sensor
  int sensorValue = analogRead(SOIL_PIN);
  
  // Print the value to the Serial Monitor
  Serial.print("Analog Reading: ");
  Serial.println(sensorValue);
  
  // Wait for 1 second before the next reading
  delay(1000);
}
