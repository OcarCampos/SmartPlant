#include <LiquidCrystal.h>

/*
LiquidCrystal.h Documentation

SYNTAX
LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
LiquidCrystal(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7)
LiquidCrystal(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7)

PARAMETERS

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

Output of sensor varies from 0 to 4.28 Volts,
It's equivalent ADC value is 0 to 877 ( (4.28/5)*1024 = 877 )

Yet these are by the book values. Tests must be done to detect
what sensor measures as for dry and register the values
same for water value or totally wet soil.

Values must be registered to calibrate the variables below.

Use script MoistureCalibrate.ino to calibrate the sensor.
*/

const int AirValue = 0;         //Must be updated according to calibration.
const int WaterValue = 876;     //Must be updated according to calibration.

//Initialization of moisture values
int soilMoistureValue = 0;
int soilMoisturePercent=0;

//Setting up the variables to run in the card.
void setup() {
    Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
    lcd.begin(16, 2); //Indicate catode, anode for LCD screen.
}

//Running code.
void loop() {
    soilMoistureValue = analogRead(A0);  //Sensor connected to pin A0
    Serial.println(soilMoistureValue); 

    //Calculating percentage according to calibration.
    soilMoisturePercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

    //Printing to Serial Monitor and LCD.
    if(soilMoisturePercent >= 100)
    {
        Serial.println("100 %");
        lcd.setCursor(0, 0);
        lcd.print("Soil Moisture");
        lcd.setCursor(0, 1);
        lcd.print("100 %");
        delay(250);
        lcd.clear();
    }
    else if(soilMoisturePercent <=0)
    {
        Serial.println("0 %");
        lcd.setCursor(0, 0);
        lcd.print("Soil Moisture");
        lcd.setCursor(0, 1);
        lcd.print("0 %");
        delay(250);
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
        delay(250);
        lcd.clear();
    }  
}