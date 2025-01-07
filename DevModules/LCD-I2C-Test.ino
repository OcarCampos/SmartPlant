#include <Wire.h>               //Library for I2C
#include <LiquidCrystal_I2C.h>  //Library for the LCD screen.

LiquidCrystal_I2C lcd(0x27,16,2);       //0x27 is for Arduino Uno R4 wifi as per i2c-scanner.ino

void setup() {
    Serial.begin(9600);         // open serial port, set the baud rate to 9600 bps    
    
    lcd.init();           //Indicate the type of LCD we are using and start the screen.
    lcd.backlight();      //Turn on the backlight

    // Welcome message in Serial Monitor
    Serial.println("\*\*\*\*\*\*\*\*\*\*\*\*\*");
    Serial.println("Welcome to Smart Plant 1.0");
    Serial.println("\*\*\*\*\*\*\*\*\*\*\*\*\*");

    // Welcome message in LCD
    lcd.setCursor(0,0);
    lcd.print("Welcome to");
    lcd.setCursor(0,1);
    lcd.print("Smart Plant 1.0");
    delay(3000);
    lcd.clear();
}

void loop() {
    Serial.println("Hello, World in serial console!");

    lcd.setCursor(0,0);
    lcd.print("Hello, World!");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Arduino");
    lcd.setCursor(0,1);
    lcd.print("Proteus");
    delay(1000);
    lcd.clear();
}

