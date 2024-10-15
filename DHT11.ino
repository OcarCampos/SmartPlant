#include <DHT.h>        //Library for DHT11
#define DHT11_PIN 8     //Pin for DHT11

DHT dht11(DHT11_PIN, DHT11);    //Object for DHT11

void setup() { 
    Serial.begin(9600);     // open serial port, set the baud rate to 9600 bps
    dht11.begin();          //Initialize DHT11 sensor
}

void loop() {
    float h = dht11.readHumidity();     //read humidity
    float t = dht11.readTemperature();  //read temperature

    //Check if any reading is not available
    if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT11 sensor!");
        return;
    }

    //Print temperature and humidity
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C ");
    delay(3000);
}