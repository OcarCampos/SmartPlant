/*
Arduino Program for Proteus Testing
*/

//Libraries
#include <LiquidCrystal.h>      //Library for the LCD screen.
#include <DHT.h>                //Library for DHT11 sensor.

//Pins used in the card by the sensors.
#define dht11SensorPin 8     //Pin for DHT11
#define lightSensorPin A0    //Pin for Light Sensor (LDR)

/*
 * Objects
 */
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);    //Object for LiquidCrystal(rs, enable, d4, d5, d6, d7)
DHT dht11(dht11SensorPin, DHT11);       //Object for DHT11

/*
 * Calibration values for the sensors
 */
 int day = 500;

// Time delays in ms.
int timeFrameStart = 500;
int timeFrameReadings = 500;

/*
 * Global Variables
 */

// Variables for DHT11 Sensor
float humidity = 0.0;
float temperature = 0.0;

// Variable for LDR sensor
int lightValue = 0;

/*
 * Setting up the variables to run in the card.
 */
void setup() {
    Serial.begin(9600);         // open serial port, set the baud rate to 9600 bps
    //LCD initialization
    lcd.begin(16, 2);           //Indicate the type of LCD we are using and start the screen.
    lcd.print("Smart Plant 1.0");
    Serial.println("Smart Plant 1.0");
    //Delay before clearing the screen
    delay(timeFrameStart);
    lcd.clear();
    //DHT11 Initialization
    dht11.begin();

    //for debugging only
    Serial.print("lightValue: ");
    Serial.println(lightValue);

}

/*
 * Running code.
 */
void loop() {
    // Reading DHT11 values
    humidity = dht11.readHumidity();
    temperature = dht11.readTemperature(); //in celcius

    // Reading LDR values
    lightValue = analogRead(lightSensorPin);
    
    /*
     *Printing to Serial Monitor
     */
    
    // Humidity and Temperature
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");
    delay(timeFrameReadings);

    //for debugging only
    Serial.print("lightValue: ");
    Serial.println(lightValue);


    // Light Photoresistor
    if (lightValue >= day){
        Serial.println("Light: Day Time.");
    }
    else{
        Serial.println("Light: Night Time.");
    }
    delay(timeFrameReadings);

    /*
     *Printing to LCD
     */

    //Humidity
    lcd.setCursor(0,0);
    lcd.print("Humidity:");
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
}