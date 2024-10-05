#include <LiquidCrystal.h>
// Sensor pins
#define moistSensorPin A0
#define tmpSensorPin A1
#define lightSensorPin A2
// Reference Voltage to PIN ARef
#define aref_voltage 3.3

/*
LiquidCrystal.h Documentation

SYNTAX:

LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
LiquidCrystal(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7)
LiquidCrystal(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7)

PARAMETERS:

rs:     the number of the Arduino pin that is connected to the RS pin on the LCD
rw:     the number of the Arduino pin that is connected to the RW pin on the LCD (optional)
enable: the number of the Arduino pin that is connected to the enable pin on the LCD

d0, d1, d2, d3, d4, d5, d6, d7:     the numbers of the Arduino pins that are connected to 
    the corresponding data pins on the LCD. d0, d1, d2, and d3 are optional; if omitted, 
    the LCD will be controlled using only the four data lines (d4, d5, d6, d7).
*/
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/*
Global variables

Moisture Sensor:
----------------

Output of sensor varies from 0 to 4.28 Volts,
It's equivalent ADC value is 0 to 877 ( (4.28/5)*1024 = 877 )

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

//Moisture Sensor Calibration Values
const int AirValue = 0;         //Must be updated according to calibration.
const int WaterValue = 876;     //Must be updated according to calibration.

//Photo Resistor Calibration Values
const int day = 500;

// Initialization of Sensor Values

// Moisture Values
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

// Temperature Values
int tmpValue = 0;
float voltageOut = 0.0;
float tempCelsius = 0.0;

// Light Photoresistor
int photoPeriod = 0;

// Time delay in ms.
int timeFrameStart = 2000;
int timeFrame = 1000;


// Setting up the variables to run in the card.
void setup() {
    Serial.begin(9600);         // open serial port, set the baud rate to 9600 bps
    analogReference(EXTERNAL);  // Set the aref to external source
    lcd.begin(16, 2);           //Indicate catode, anode for LCD screen.
    lcd.print("Smart Plant 1.0");
    delay(timeFrameStart);
    lcd.clear();
}

// Running code.
void loop() {
    // Reading Pin Values
    soilMoistureValue = analogRead(moistSensorPin);
    tmpValue = analogRead(tmpSensorPin);
    photoPeriod = analogRead(lightSensorPin);

    // Printing control values to serial monitor
    Serial.print("Moisture Value: ");
    Serial.println(soilMoistureValue);
    Serial.print("Temperature Value: ");
    Serial.println(tmpValue);
    Serial.print("Photo Resistor Value: ");
    Serial.println(photoPeriod);

    soilMoisturePercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);     // Calculating moisture percentage according to calibration values

    voltageOut = tmpValue * (aref_voltage / 1024.0);      // Convert temp sensor reading into voltage
    tempCelsius = (voltageOut - 0.5) * 100;                 // Temp in Celsius converted from voltage


    // Defining if light's are on or off.
    lcd.setCursor(0,0);
    lcd.print("Photoperiod");
    lcd.setCursor(0,1);
    if (photoPeriod >= day){
        //it's day time!
        Serial.println("Photoperiod: Day Time.");   
        lcd.print("Day Time.");
    }
    else {
        //it's night time!
        Serial.println("Photoperiod: Night Time.");
        lcd.print("Night Time.");
    }
    delay(timeFrame);
    lcd.clear();
    
    // Print the temperature in Celsius in serial monitor 
    Serial.print("Temperature Celsius: "); 
    Serial.print(tempCelsius); 
    Serial.print("\xB0"); // prints the degree symbol 
    Serial.println("C"); 

    // Print the temperature on the LCD
    lcd.setCursor(0, 0);
    lcd.print("Temperature");
    lcd.setCursor(0, 1);
    lcd.print(tempCelsius, 2);
    lcd.setCursor(5, 1);
    lcd.print("\xB0 C");
    delay(timeFrame);
    lcd.clear();


    //Printing to Serial Monitor and LCD soil moisture percentage.
    Serial.print("Moisture Percentage Value: ");
    if(soilMoisturePercent >= 100)
    {
        Serial.println("100 %");
        lcd.setCursor(0, 0);
        lcd.print("Soil Moisture:");
        lcd.setCursor(0, 1);
        lcd.print("100 %");
        delay(timeFrame);
        lcd.clear();
    }
    else if(soilMoisturePercent <=0)
    {
        Serial.println("0 %");
        lcd.setCursor(0, 0);
        lcd.print("Soil Moisture");
        lcd.setCursor(0, 1);
        lcd.print("0 %");
        delay(timeFrame);
        lcd.clear();
    }
    else if(soilMoisturePercent >0 && soilMoisturePercent < 100)
    {
        Serial.print(soilMoisturePercent);
        Serial.println("%");
        lcd.setCursor(0, 0);
        lcd.print("Soil Moisture");
        lcd.setCursor(0, 1);
        lcd.print(soilMoisturePercent);
        lcd.print(" %");
        delay(timeFrame);
        lcd.clear();
    }
    Serial.println("\*\*\*\*\*\*");
}