//Setting up the variables to run in the card.
void setup()
{
    // Set the serial monitor baudrate to 9600
    // This is to be able to monitor the output in the serial monitor
    Serial.begin(9600);
  
    
    pinMode(13, OUTPUT); //Output LED Level 1 Moisture Digital Pin 13
    pinMode(12, OUTPUT); //Output LED Level 2 Moisture Digital Pin 12
    pinMode(11, OUTPUT); //Output LED Level 3 Moisture Digital Pin 11
    pinMode(10, OUTPUT); //Output LED Level 4 Moisture Digital Pin 10
    pinMode(9, OUTPUT); //Output LED Level 5 Moisture Digital Pin 9
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
    //delay(2000);
  
    // Turn off All LEDs
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
    digitalWrite(9, LOW);
  
    // output of sensor varies from 0 to 4.28 Volts,
    // It's equivalent ADC value is 0 to 877 ( (4.28/5)*1024 = 877 )

    // Splitting 877 into 5 level => 175, 350, 525, 700, 877
    // Based on the ADC output, LED indicates the level (1 to 5).
    // Being 1 low moisture which means require water
    // Being 5 hig moisture which means enough water in the system
  
    if (moistureLevel <= 150) {
        // LEVEL 1 LED
        // System needs water urgently
        digitalWrite(13, HIGH);
    }
    else if (moistureLevel > 150 && moistureLevel <= 350) {
        // LEVEL 2 LED
        // System needs water
        digitalWrite(12, HIGH);
    }
    else if (moistureLevel > 350 && moistureLevel <= 500) {
        // LEVEL 3 LED
        // System is getting dry
        digitalWrite(11, HIGH);
    }
    else if (moistureLevel > 500 && moistureLevel <= 750) {
       // LEVEL 4 LED
       // System is hidrated
        digitalWrite(10, HIGH);
    }
    else if (moistureLevel > 750) {
        // LEVEL 5 LED
        // System is over hydrated
        digitalWrite(9, HIGH);
    }
}