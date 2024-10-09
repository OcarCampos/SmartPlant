#include <LiquidCrystal.h>

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
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// Time delay in ms.
int timeFrameStart = 2000;
int timeFrame = 1000;

// Setting up the variables to run in the card.
void setup() {
    Serial.begin(9600);         // open serial port, set the baud rate to 9600 bps
    lcd.begin(16, 2);           //Indicate the type of LCD we are using.
    lcd.print("Smart Plant 1.0");
    delay(timeFrameStart);
    lcd.clear();
}

void loop() {
    // Reading Pin Values
}