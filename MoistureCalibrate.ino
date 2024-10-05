//Setting up the variables to run in the card.
void setup()
{
    // Set the serial monitor baudrate to 9600
    // This is to be able to monitor the output in the serial monitor
    Serial.begin(9600);
}

void loop()
{
    // Variable to store ADC value 
    // ADC varies based on voltage( 0 to 1023 )
    int moistureLevel;
    // analogRead function returns the integer 10 bit integer (0 to 1023)
    // Read pin A0 where moisture sensor is connected
    moistureLevel = analogRead(0); 
  
    // Print text in serial monitor
    Serial.println("Moisture Value: ");
    // Print analog value in serial monitor
    Serial.println(moistureLevel);
    //Set a delay of 2 seconds to space the reading
    delay(2000);
}