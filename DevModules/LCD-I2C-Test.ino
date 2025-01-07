/*
Script for testing the LCD screen with I2C connection
Last modified: 01/07/2025
Working with Arduino Uno R4 wifi
*/

#include <Wire.h>               //Library for I2C
#include <LiquidCrystal_I2C.h>  //Library for the LCD screen.

LiquidCrystal_I2C lcd(0x27,16,2);       //0x27 is for Arduino Uno R4 wifi as per i2c-scanner.ino

// Setup of the script
void setup() {
    Serial.begin(9600);    // open serial port, set the baud rate to 9600 bps    
    
    lcd.init();           //Indicate the type of LCD we are using and start the screen.
    lcd.backlight();      //Turn on the backlight

    // Welcome message in Serial Monitor
    Serial.println("\*\*\*\*\*\*\*\*\*\*\*\*\*");
    Serial.println("Welcome to LCD-I2C-Test");
    Serial.println("\*\*\*\*\*\*\*\*\*\*\*\*\*");

    // Welcome message in LCD
    lcd.setCursor(0,0);
    lcd.print("Welcome to");
    lcd.setCursor(0,1);
    lcd.print("LCD-I2C-Test");
    delay(3000);
    lcd.clear();
}

// Loop of the script
void loop() {
    // Printing to serial monitor
    Serial.println("Hello, World in serial console!");

    // Printing to LCD
    lcd.setCursor(0,0);
    lcd.print("Hello, World!");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Arduino");
    lcd.setCursor(0,1);
    lcd.print("Test");
    delay(1000);
    lcd.clear();
}

