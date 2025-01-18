/*
Script for the calibration of Soil Moisture Sensor
Last modified: 01/07/2025
Working with Arduino Uno R4 wifi
Sensors used: Custom Soil Moisture sensor made out of lead from a pencil.
Any resistive soil moisture sensor can be used. Before the lead pencil one
I used one from aliexpress:
https://www.aliexpress.com/item/1005006344637182.html
*/

#define moistSensorPin A0 // Pin for moisture sensor

/*
 * Soil Moisture Sensor:
 * ----------------------

 * Output of sensor is either 0 (water) or 1023 (air) by theory.
 *
 * Must be calibrated with this script to be able to read extreme conditions
 * and validate that 0 is water and 1023 is air.
 *
 * Calibration with small pot with soil
 *
 * 266 = full in water
 * 1021 = full in empty air
 
*/


// Setting up the script
void setup()
{
    Serial.begin(9600);
}

// Recurrent loop of the script
void loop()
{
    // Variable to store ADC value 
    // ADC varies based on voltage( 0 to 1023 )
    int moistureLevel;
    // analogRead function returns the integer 10 bit integer (0 to 1023)
    // Read pin A0 where moisture sensor is connected
    moistureLevel = analogRead(moistSensorPin); 
  
    // Print text in serial monitor
    Serial.println("Moisture Value: ");
    // Print analog value in serial monitor 
    Serial.println(moistureLevel);
    //Set a delay of 2 seconds to space the reading
    delay(5000);
}