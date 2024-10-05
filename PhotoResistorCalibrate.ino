// Sensor pins
#define lightSensorPin A2

/*
Global variables

Photo Resistor Sensor:
----------------------

Output of sensor varies from 4.74 (night) Volts to 241 mV (day).
It's equivalent ADC value is to 82 (night) to 1023 (day).
Any value above 82 means it's day time.

Calibration:
------------

Above values are simulation values. Tests must be done to detect
what sensors measure to register real values.

This must be done to calibrate variables below.

MoistureCalibrate.ino: to calibrate the moisture sensor.
PhotoResistorCalibrate.ino: to calibrate photo resistor.
*/


//Photo Resistor Calibration Values
const int day = 500;

// Initialization of Sensor Values

// Light Photoresistor
int photoPeriod = 0;

// Time delay in ms.
int timeFrame = 1000;


// Setting up the variables to run in the card.
void setup() {
    Serial.begin(9600);         // open serial port, set the baud rate to 9600 bps
}

// Running code.
void loop() {
    // Reading Pin Values
    photoPeriod = analogRead(lightSensorPin);

    // Printing control values to serial monitor
    Serial.print("Photo Resistor Value: ");
    Serial.println(photoPeriod);

    // Defining if light's are on or off.
    if (photoPeriod >= day){
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