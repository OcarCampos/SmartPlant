/*
Arduino Program for SmartPlant Testing.
Last modified: 12/15/2024
Working with Arduino Uno R4 wifi
Sensors used: DHT11, LDR, Moisture
LCD 16x2 with I2C adapter.
Water pump and relay used.
Connection to Wifi and ThingSpeak using secrets.h
*/

//Libraries
#include <WiFiS3.h>             // WiFi library for arduino uno
#include "arduino_secrets.h"    // Secrets file with credentials
#include <Wire.h>               //Library for I2C
#include <LiquidCrystal_I2C.h>  //Library for the LCD screen.
#include <DHT.h>                //Library for DHT11 sensor.
#include <ThingSpeak.h>         //Library for ThingSpeak

//Pins used in the card by the sensors.
#define dht11SensorPin 8     //Pin for DHT11 - Digital Pin
#define lightSensorPin 9     //Pin for Light Sensor (LDR) - Digital Pin
#define relayPin 2           //Pin for Relay - Digital Pin
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
 * WiFi Credentials
 */
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;       // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status

/*
 * ThingSpeak Credentials
 */
unsigned long myChannelNumber = SECRET_CH_ID;       // ThingSpeak Channel ID
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;   // ThingSpeak Write API Key
String myStatus = "";                               // ThingSpeak status for channel
WiFiClient client;                                  // WiFi client object for ThingSpeak connection

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
const int WaterValue = 1000;     // According to calibration
const int AirValue = 330;       // According to calibration
const int moistureLimit = 45;   // Moisture limit for watering plants. Value in % according to calibration.


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

// Variable for water pump usage
int waterCycles = 0;             // Number of water cycles done by the pump

// Variables for relay state. Usually High is on and Low is off
// But in this case after testing we found that High is off and Low is on
const byte relayOn = LOW;
const byte relayOff = HIGH;

/*
 * Time Frames in ms.
 * 1 second is 1000 milliseconds
 * 5 minutes are 300000 milliseconds
 * 10 minutes are 600000 milliseconds
 * 15 minutes are 900000 milliseconds
 * 20 minutes are 1200000 milliseconds
 * 25 minutes are 1500000 milliseconds
 * 30 minutes are 1800000 milliseconds
 * 35 minutes are 2100000 milliseconds
 * 40 minutes are 2400000 milliseconds
 * 45 minutes are 2700000 milliseconds
 * 50 minutes are 3000000 milliseconds
 * 55 minutes are 3300000 milliseconds
 * 60 minutes are 3600000 milliseconds
 */
unsigned long previousMillis = 0;           // Will store last time information was sent to thingspeak
const int intervalStart = 5000;              //5 seconds for welcoming message
const int intervalPause = 3000;              //3 seconds for clearing the screen
const int intervalWatering = 5000;           //5 seconds for watering
const int intervalWateringPause = 20000;     //20 seconds for water to absorb
const int intervalThingSpeak = 900000;      // 15 minutes for reporting to ThingSpeak

/*
 * Setting up the variables to run in the card.
 */
void setup() {
    Serial.begin(9600);         // open serial port, set the baud rate to 9600 bps, not needed in production
    
    // LCD initialization
    lcd.init();                 //Indicate the type of LCD we are using and start the screen.
    lcd.backlight();            //Turn on the backlight

    // Welcome message in LCD
    lcd.setCursor(0,0);
    lcd.print("Welcome to");
    lcd.setCursor(0,1);
    lcd.print("Smart Plant 1.0");
    delay(intervalStart);
    lcd.clear();

    // Connect to WiFi
    lcd.setCursor(0,0);
    lcd.print("Connecting WiFi");
    lcd.setCursor(0,1);
    lcd.print("Please wait...");
    connect_to_wifi();                  // Try to connect to the wifi given the credentials
    lcd.clear();

    // Sensors Initialization
    lcd.setCursor(0,0);
    lcd.print("Starting sensors");
    lcd.setCursor(0,1);
    lcd.print("Please wait...");
    dht11.begin();                      // DHT11 Initialization
    pinMode(lightSensorPin, INPUT);     // LDR Pin Initialization
    pinMode(relayPin, OUTPUT);          // Relay Pin Initialization
    digitalWrite(relayPin, relayOff);   // Set relay to off by default
    ThingSpeak.begin(client);           // ThingSpeak Initialization
    delay(intervalStart);
    lcd.clear();

    // Init done
    lcd.setCursor(0,0);
    lcd.print("Init done.");
    lcd.setCursor(0,1);
    lcd.print("Starting loop.");
    delay(intervalStart);              // Delay before moving on to loop
}

/*
 * Running code.
 */
void loop() {
    /*
     * Start of readings loop
     * Initialization messages
     */
    // Check if we arer still connected to WiFi and if not, try to reconnect
    if (status != WL_CONNECTED) {
        lcd.setCursor(0,0);                 
        lcd.print("Reconnecting WiFi");
        lcd.setCursor(0,1);
        lcd.print("Please wait...");
        connect_to_wifi();                  // Try to connect to the wifi given the credentials
        lcd.clear();
    }

    // LCD initialization message
    lcd.setCursor(0,0);                 
    lcd.print("SmartPlant 1.0");
    lcd.setCursor(0,1);
    lcd.print("Reading sensors");
    // Delay before clearing LCD screen
    delay(intervalPause);
    lcd.clear();

    /*
     * Reading Sensor Values
     */
    sensor_readings();    

    /*
     * Printing Sensor Values both to serial monitor and LCD
     */
    serial_monitor_printing();  //not needed in production.
    lcd_screen_printing();

    /*
     * Checking if the plant needs to be watered
     */
    watering_check();

    /*
     * Sending Sensor Values to ThingSpeak
     */
    unsigned long currentMillis = millis();     // Get current time
    // If time passed is greater than interval to send to ThingSpeak
    if (currentMillis - previousMillis >= intervalThingSpeak) {
        previousMillis = currentMillis; // Update previousMillis
        send_to_thing_speak();          // Send sensor values to ThingSpeak
    }

    /*
     * End of readings loop
     */	
    // Printing to Serial Monitor Ending readings
    Serial.println("\*\*\*\*\*\*\*\*\*\*\*\*\*"); // not needed in production

    delay(intervalPause); // delay to start the loop again
}

/*
 * Function that reads the sensors values
 * and assigns them to their respective variables
 * All assigned variables are global variables, so they can be used in other functions.
*/
void sensor_readings() {
    // Reading DHT11 values
    humidity = dht11.readHumidity();        // Read humidity
    temperature = dht11.readTemperature();  // Read temperature

    // Reading LDR values
    photoPeriod = digitalRead(lightSensorPin); // Read light sensor value

    // Reading Moisture values
    soilMoistureValue = analogRead(moistSensorPin);
    // Calculate moisture percentage according to calibration values
    soilMoisturePercent = map(soilMoistureValue, WaterValue, AirValue, 0, 100);     
}

/*
 * Function that prints messages to the serial monitor for debugging.
 */
void serial_monitor_printing() {
    // Humidity and temperature
    Serial.print("Air Humidity: ");
    Serial.print(humidity);
    Serial.println(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");

    // Light Photoresistor and Photoperiod
    Serial.print("LDR Raw Value: ");
    Serial.println(photoPeriod);
    // Photoperiod
    if (photoPeriod == day){
        Serial.println("Light: Day Time.");
    }
    else{
        Serial.println("Light: Night Time.");
    }

    // Moisture
    Serial.print("Raw Moisture Value: ");
    Serial.println(soilMoistureValue);
    // Moisture Percent Value
    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisturePercent);
    Serial.println("%");
}

/*
 * Function that prints messages to the LCD screen to keep the user
 * up to date with the readings.
 */
void lcd_screen_printing() {
    // DHT11 Humidity
    lcd.setCursor(0,0);
    lcd.print("Air Humidity:");
    lcd.setCursor(0,1);
    lcd.print(humidity);
    lcd.print("%");
    delay(intervalPause);
    lcd.clear(); 

    // DHT11 Temperature
    lcd.setCursor(0,0);
    lcd.print("Temperature:");
    lcd.setCursor(0,1);
    lcd.print(temperature);
    lcd.print("*C");
    delay(intervalPause);
    lcd.clear();

    // LDR value
    // Day time
    if (photoPeriod == day){
        lcd.setCursor(0,0);
        lcd.print("Light:");
        lcd.setCursor(0,1);
        lcd.print("Day Time.");
    }
    // Night time
    else{
        lcd.setCursor(0,0);
        lcd.print("Light:");
        lcd.setCursor(0,1);
        lcd.print("Night Time.");
    }
    delay(intervalPause);
    lcd.clear();

    // Soil Moisture
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
    delay(intervalPause);
    lcd.clear();
}
/*
 * Function that checks if the plants needs to be watered
 * depending on the readings from the soil moisture sensor.
 *
 * If moisture is lower than 40% function will turn on the
 * relay for intervalWatering seconds and then turn off the relay.
 *
 * Function will wait for intervalWateringPause seconds to allow for
 * water to be absorved by soil before turning on the relay again
 * watering the plants a second cycle.
 *
 * Function will repeat this process a third cycle and move on.
 *
 * Total watering time is intervalWatering*3 + intervalWateringPause*2
 */
void watering_check(){
    // If moisture is less than moistureLimit we pump water
    if(soilMoisturePercent <= moistureLimit){
        // Printing to Serial Monitor
        Serial.println("Moisture lower than 40%. Watering Plants."); // Not needed in production
        lcd.setCursor(0,0);
        lcd.print("Watering Plants.");
        lcd.setCursor(0,1);
        lcd.print("1st cycle...");
        // First cycle. Turn on the relay.
        digitalWrite(relayPin, relayOn);
        delay(intervalWatering);
        // Turn off the relay. Wait for water to absorb.
        digitalWrite(relayPin, relayOff);
        delay(intervalWateringPause);
        // Second cycle. Turn on the relay.
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Watering Plants.");
        lcd.setCursor(0,1);
        lcd.print("2nd cycle...");
        digitalWrite(relayPin, relayOn);
        delay(intervalWatering);
        // Turn off the relay. Wait for water to absorb.
        digitalWrite(relayPin, relayOff);
        delay(intervalWateringPause);
        // Third cycle. Turn on the relay.
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Watering Plants.");
        lcd.setCursor(0,1);
        lcd.print("3rd cycle...");
        digitalWrite(relayPin, relayOn);
        delay(intervalWatering);
        // Turn off the relay. Wait for water to absorb.
        digitalWrite(relayPin, relayOff);
        delay(intervalWateringPause);
        Serial.println("Watering cycle complete."); // Not needed in production
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Watering Plants.");
        lcd.setCursor(0,1);
        lcd.print("Complete...");
        waterCycles++;
    }
    // Make sure relay is off if condition is not met
    else{
        digitalWrite(relayPin, relayOff);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Plants Hydrated.");
        lcd.setCursor(0,1);
        lcd.print("No water needed.");
        delay(intervalPause);
        lcd.clear();
        Serial.println("Moisture higher than 40%. No watering."); // Not needed in production
    }
}

/* 
 * Function that sends the sensor values to ThingSpeak
 * Must be configured according to the channel
 * and the API key of the channel
 *
 * Data to send to channel:
 * 0. Status message
 * 1. Temperature from DHT11 in *C	
 * 2. Humidity from DHT11 in %
 * 3. Photoperiod from LDR (0 for night time and 1 for day time)
 * 4. Moisture from Moisture sensor in %
 * 5. Water Cycles done by the pump until now.
 */
void send_to_thing_speak() {
    // Set the fields with the values
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);
    // Photoperiod is 0 for day and 1 for night
    if (photoPeriod == day){
        // 1 for lamps on.
        ThingSpeak.setField(3, 1);
    }
    else{
        // 0 for lamps off
        ThingSpeak.setField(3, 0);
    }
    ThingSpeak.setField(4, soilMoisturePercent);
    ThingSpeak.setField(5, waterCycles);

    // Set the status message
    myStatus = "Data updated successfully!";
    ThingSpeak.setStatus(myStatus);

    // Write to ThingSpeak channel.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    delay(1000); // Give some time for the connection to complete
    // Report result of connection to ThingSpeak
    if (x == 200 || x == 0) {
        Serial.println("Channel update successful!"); // not needed in production
        lcd.setCursor(0,0);
        lcd.print("ThingSpeak has");
        lcd.setCursor(0,1);
        lcd.print("been updated.");
    }
    else {
        Serial.println("Problem updating channel. HTTP error code " + String(x)); // not needed in production
        lcd.setCursor(0,0);
        lcd.print("ThingSpeak has");
        lcd.setCursor(0,1);
        lcd.print("not been updated.");
    }
    delay(intervalPause);
    lcd.clear();

    //reset water cycles to 0
    waterCycles = 0;
}

/*
 * Function that connects to the wifi given the credentials
 */
void connect_to_wifi() {
    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
    }
    // check wifi firmware version
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
    }
    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network:
        status = WiFi.begin(ssid, pass);
        // wait 10 seconds for connection:
        delay(10000);
    }
    // you're connected now, so print out the data:
    Serial.println("WiFi connection successful!");
    // print to LCD
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WiFi Connected.");
    lcd.setCursor(0,1);
    lcd.print("Continuing...");
    printCurrentNet();
    printWifiData();
    delay(intervalStart); // Wait for 5 seconds before exiting
}

/*
 * Function that prints the wifi data
 */
void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

/*
 * Function that prints the current network to which the board is connected.
 */
void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
}