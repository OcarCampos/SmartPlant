/*
Script for testing several sensors with the Proteus software.
Last modified: 01/07/2025
Working with Simulino Uno in Proteus, DHT11, LDR and Moisture Sensor
*/

//Libraries
#include <LiquidCrystal.h>      //Library for the LCD screen.
#include <DHT.h>                //Library for DHT11 sensor.

//Pins used in the card by the sensors.
#define dht11SensorPin 8     //Pin for DHT11
#define lightSensorPin A0    //Pin for Light Sensor (LDR)
#define moistSensorPin A1    //Pin for Moisture Sensor

/*
 * Objects
 */
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);    //Object for LiquidCrystal(rs, enable, d4, d5, d6, d7)
DHT dht11(dht11SensorPin, DHT11);       //Object for DHT11

/*
 *Calibration values for the sensors
 */

 //Calibration value for LDR sensor
const int day = 500;            //Must be updated according to calibration.

//Calibration values for the moisture sensor
const int AirValue = 0;          //Must be updated according to calibration.
const int WaterValue = 1019;     //Must be updated according to calibration.

/*
 * Global Variables
 */

// Variables for DHT11 Sensor
float humidity = 0.0;
float temperature = 0.0;

// Variable for LDR sensor
int lightValue = 0;

// Variables for moisture sensor
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

/*
 * Time Frames in ms.
 */
int timeFrameStart = 1000;
int timeFrameReadings = 1000;

/*
 * Setting up the variables to run in the card.
 */
void setup() {
    Serial.begin(9600);         // open serial port, set the baud rate to 9600 bps
    
    //LCD initialization
    lcd.begin(16, 2);           //Indicate the type of LCD we are using and start the screen.

    //Welcome message in LCD
    lcd.setCursor(0,0);
    lcd.print("Welcome to");
    lcd.setCursor(0,1);
    lcd.print("Smart Plant 1.0");

    // Welcome message in Serial Monitor
    Serial.println("\*\*\*\*\*\*\*\*\*\*\*\*\*");
    Serial.println("Welcome to Smart Plant 1.0");
    Serial.println("\*\*\*\*\*\*\*\*\*\*\*\*\*");

    //Delay before clearing the screen
    delay(timeFrameStart);
    lcd.clear();
    
    //DHT11 Initialization
    dht11.begin();
}

/*
 * Running code.
 */
void loop() {
    //Serial Monitor init message
    Serial.println("Smart Plant 1.0");

    // Reading DHT11 values
    humidity = dht11.readHumidity();
    temperature = dht11.readTemperature(); //in celcius

    // Reading LDR values
    lightValue = analogRead(lightSensorPin);

    // Reading Moisture values
    soilMoistureValue = analogRead(moistSensorPin);
    soilMoisturePercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);     // Calculating moisture percentage according to calibration values
    
    /*
     *Printing to Serial Monitor
     */
    
    // Humidity and Temperature
    Serial.print("Air Humidity: ");
    Serial.print(humidity);
    Serial.println(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");

    // Light Photoresistor
    Serial.print("Raw lightValue: ");
    Serial.println(lightValue);
    // Photoperiod
    if (lightValue >= day){
        Serial.println("Light: Day Time.");
    }
    else{
        Serial.println("Light: Night Time.");
    }

    // Moisture
    Serial.print("Raw Moisture Value: ");
    Serial.println(soilMoistureValue);
    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisturePercent);
    Serial.println("%");

    // Ending readings
    Serial.println("\*\*\*\*\*\*\*\*\*\*\*\*\*");

    // Time delay before printing to LCD
    delay(timeFrameReadings);

    /*
     *Printing to LCD
     */

    //LCD initialization message
    lcd.setCursor(0,0);
    lcd.print("Smart Plant 1.0");
    lcd.setCursor(0,1);
    lcd.print("Reading sensors");

    //Delay before clearing the screen
    delay(timeFrameReadings);
    lcd.clear();


    //Humidity
    lcd.setCursor(0,0);
    lcd.print("Air Humidity:");
    lcd.setCursor(0,1);
    lcd.print(humidity);
    lcd.print("%");
    //Delay before clearing the screen
    delay(timeFrameReadings);
    lcd.clear();    

    //Temperature
    lcd.setCursor(0,0);
    lcd.print("Temperature:");
    lcd.setCursor(0,1);
    lcd.print(temperature);
    lcd.print("*C");
    //Delay before clearing the screen
    delay(timeFrameReadings);
    lcd.clear();

    // Light Photoresistor
    if (lightValue >= day){
        lcd.setCursor(0,0);
        lcd.print("Light:");
        lcd.setCursor(0,1);
        lcd.print("Day Time.");
    }
    else{
        lcd.setCursor(0,0);
        lcd.print("Light:");
        lcd.setCursor(0,1);
        lcd.print("Night Time.");
    }
    //Delay before clearing the screen
    delay(timeFrameReadings);
    lcd.clear();

    // Moisture
    lcd.setCursor(0,0);
    lcd.print("Soil Moisture:");
    lcd.setCursor(0,1);
    lcd.print(soilMoisturePercent);
    lcd.print("%");
    //Delay before clearing the screen
    delay(timeFrameReadings);
    lcd.clear();
}