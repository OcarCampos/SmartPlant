// Sensor pins
#define lightSensorPin 9

/*
 * Photo Resistor Sensor:
 * ----------------------

 * Output of sensor is either 1 or 0.
 * 0 = day time/light is on
 * 1 = night time/light is off
*/

//Variables
int day = 0;
int photoPeriod = 0;        //starting photoperiod as day time by default
int timeFrame = 1000;       //time frame between each reading.



// Setting up the variables to run in the card.
void setup() {
    Serial.begin(9600);         // open serial port, set the baud rate to 9600 bps
    Serial.println("Starting LDR calibration....");
    pinMode(lightSensorPin, INPUT);
}

// Running code.
void loop() {
    // Reading Pin Values
    photoPeriod = digitalRead(lightSensorPin);

    // Printing control values to serial monitor
    Serial.print("Photo Resistor Value: ");
    Serial.println(photoPeriod);

    // Defining if light's are on or off.
    if (photoPeriod == day){
        //it's day time!
        Serial.println("Photoperiod: Day Time.");   
    }
    else {
        //it's night time!
        Serial.println("Photoperiod: Night Time.");
    }
    delay(timeFrame);
    Serial.println("\*\*\*\*\*\*");
}