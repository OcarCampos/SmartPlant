/*
Arduino Program for SmartPlant
Last modified: 11/30/2024
Working with Arduino Uno R4 wifi
Sensors used: DHT11, LDR, Moisture
LCD 16x2 with I2C adapter.
*/

//Libraries
#include <Wire.h>               //Library for I2C
#include <LiquidCrystal_I2C.h>  //Library for the LCD screen.
#include <DHT.h>                //Library for DHT11 sensor.

//Pins used in the card by the sensors.
#define dht11SensorPin 8     //Pin for DHT11 - Digital Pin
#define lightSensorPin 9     //Pin for Light Sensor (LDR) - Digital Pin
#define relayPin 10          //Pin for Relay - Digital Pin
#define moistSensorPin A0    //Pin for Moisture Sensor - Analog Pin

/*
 * Objects
 */

// Creates LCD object with address and 16 columns x 2 rows
// Depending on where it will be executed, next line may need to be uncommented or commented
LiquidCrystal_I2C lcd(0x27,16,2);       //0x27 is for Arduino Uno R4 wifi as per i2c-scanner.ino
//LiquidCrystal_I2C lcd(0x20,16,2);        //0x20 is for I2C simulation in Proteus Software
DHT dht11(dht11SensorPin, DHT11);       //Object for DHT11

/*
 * Calibration values for the sensors
 */

/*
 * Photo Resistor Sensor:
 * ----------------------

 * Output of sensor is either 1 or 0.
 * 0 = day time/light is on
 * 1 = night time/light is off
*/
const int day = 0;      // day time is 0

/*
 * Soil Moisture Sensor:
 * ----------------------

 * Output of sensor is either 0 (water) or 1023 (air) by theory.
 
 * Calibration results with small pot with soil.
 *
 * 100 = full in water
 * 194 = soil 100% moistured
 * 200 = soil 100% moistured
 * 200 = soil 100% moistured
 * 330 = soil 100% moistured
 * 900 = soil 100% dry
 * 1020 = full in air (in dry soil)
 
*/
const int WaterValue = 330;     // According to calibration
const int AirValue = 900;       // According to calibration


/*
 * Global Variables
 */

// Variables for DHT11 Sensor
float humidity = 0.0;
float temperature = 0.0;

// Variable for LDR sensor
int photoPeriod = 0;            //Photo period as day time by default.

// Variables for moisture sensor
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

// Variables for relay state. Usually High is on and Low is off
// But in this case after testing we found that High is off and Low is on
const byte relayOn = LOW;
const byte relayOff = HIGH;

/*
 * Time Frames in ms.
 */
int timeFrameStart = 5000;
int timeFrameReadings = 5000;

/*
 * Setting up the variables to run in the card.
 */
void setup() {
    Serial.begin(9600);         // open serial port, set the baud rate to 9600 bps
    
    // LCD initialization
    lcd.init();           //Indicate the type of LCD we are using and start the screen.
    lcd.backlight();      //Turn on the backlight

    // Welcome message in LCD
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
    
    // Sensors Initialization
    dht11.begin();                      // DHT11 Initialization
    pinMode(lightSensorPin, INPUT);     // LDR Pin Initialization
    pinMode(relayPin, OUTPUT);          // Relay Pin Initialization
    digitalWrite(relayPin, relayOff);   // Set relay to off by default
    delay(timeFrameStart);
}

/*
 * Running code.
 */
void loop() {
    /*
     * Start of readings loop
     * Initialization messages
     */

    // Serial Monitor init message
    Serial.println("Smart Plant 1.0");

    // Delay before clearing the screen.
    delay(timeFrameReadings);
    lcd.clear();
    // LCD initialization message
    lcd.setCursor(0,0);
    lcd.print("Smart Plant 1.0");
    lcd.setCursor(0,1);
    lcd.print("Reading sensors");

    /*
     * Reading Sensors
     */

    // Reading DHT11 values
    humidity = dht11.readHumidity();        // Read humidity
    temperature = dht11.readTemperature();  // Read temperature

    // Reading LDR values
    photoPeriod = digitalRead(lightSensorPin); // Read light sensor value

    // Reading Moisture values
    soilMoistureValue = analogRead(moistSensorPin);
    soilMoisturePercent = 100 - map(soilMoistureValue, WaterValue, AirValue, 0, 100);     // Calculate moisture percentage according to calibration values
    

    /*
     * Printing Sensor Values
     */
    
    /*
     *  Humidity and Temperature
     */
    // Printing to Serial Monitor
    Serial.print("Air Humidity: ");
    Serial.print(humidity);
    Serial.println(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");

    // Printing to LCD
    // Delay before clearing LCD screen
    delay(timeFrameReadings);
    lcd.clear();
    // Printing Air Humidity
    lcd.setCursor(0,0);
    lcd.print("Air Humidity:");
    lcd.setCursor(0,1);
    lcd.print(humidity);
    lcd.print("%");
    
    //Delay before clearing the screen
    delay(timeFrameReadings);
    lcd.clear();   
    // Printing Temperature
    lcd.setCursor(0,0);
    lcd.print("Temperature:");
    lcd.setCursor(0,1);
    lcd.print(temperature);
    lcd.print("*C");

    /*
     * Light Photoresistor and Photoperiod
     */
    // Printing to Serial Monitor
    Serial.print("LDR Raw Value: ");
    Serial.println(photoPeriod);
    // Photoperiod
    if (photoPeriod == day){
        Serial.println("Light: Day Time.");
    }
    else{
        Serial.println("Light: Night Time.");
    }

    // Printing to LCD
    // Delay before clearing the screen
    delay(timeFrameReadings);
    lcd.clear();

    // Printing Light Photoresistor Value
    if (photoPeriod == day){
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

    /*
     * Moisture
     */
    // Printing to Serial Monitor
    Serial.print("Raw Moisture Value: ");
    Serial.println(soilMoistureValue);
    // Moisture Percent Value
    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisturePercent);
    Serial.println("%");

    // Printing to LCD
    // Delay before clearing the screen
    delay(timeFrameReadings);
    lcd.clear();
    // Printing Moisture Value
    lcd.setCursor(0,0);
    lcd.print("Soil Moisture:");
    lcd.setCursor(0,1);
    // Moisture Percent 0 or less
    if(soilMoisturePercent <= 0){
        lcd.print("0%");
    }
    // Moisture Percent more than 0
    else if(soilMoisturePercent >= 100){
        lcd.print("100%");
    }
    // Moisture Percent between 0 and 100
    else{
        lcd.print(soilMoisturePercent);
        lcd.print("%");
    }

    /*
     * Logic for turning on or off the relay for the water pump
     */
    // If moisture is less than 40% we pump water for 10 seconds
    if(soilMoisturePercent <= 40){
        // Printing to Serial Monitor
        Serial.println("Moisture lower than 40%. Pumping water for 1 seconds.");
        // Printing to LCD
        delay(timeFrameReadings);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Plants Dry.");
        lcd.setCursor(0,1);
        lcd.print("Watering 1 sec.");
        // Turn on the relay for 1 second
        digitalWrite(relayPin, relayOn);
        delay(1000);
        digitalWrite(relayPin, relayOff);
        delay(1000);

    }
    // Make sure relay is off if condition is not met
    else{
        digitalWrite(relayPin, relayOff);
        delay(1000);
    }

    /*
     * End of readings loop
     */	

    // Printing to Serial Monitor Ending readings
    Serial.println("\*\*\*\*\*\*\*\*\*\*\*\*\*");

    // Printing to LCD Ending readings
}