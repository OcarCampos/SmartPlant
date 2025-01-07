# SmartPlant

An Arduino-based smart plant monitoring and automated watering system. This project uses various sensors to monitor plant health and environmental conditions, with data visualization through ThingSpeak and automated watering functionality.

## Features

- Temperature and humidity monitoring using DHT11 sensor
- Soil moisture monitoring with custom soil moisture sensor
- Light condition monitoring with LDR (Light Dependent Resistor)
- Automated watering system with relay-controlled water pump
- Real-time data display on 16x2 LCD screen
- Remote monitoring through ThingSpeak platform
- WiFi connectivity for data transmission

## Hardware Requirements

- Arduino Uno R4 WiFi
- DHT11 Temperature and Humidity Sensor
- LDR (Light Dependent Resistor)
- Custom Soil Moisture Sensor
- 16x2 LCD Display with I2C adapter
- Relay Module
- Water Pump
- Power Supply
- Jumper Wires
- Breadboard (optional)

## Software Dependencies

The following Arduino libraries are required:

- WiFiS3 (for Arduino Uno R4 WiFi)
- Wire (I2C communication)
- LiquidCrystal_I2C (LCD display)
- DHT (DHT11 sensor)
- ThingSpeak (IoT platform)

## Pin Configuration

- DHT11 Sensor: Digital Pin 8
- Light Sensor (LDR): Digital Pin 9
- Relay: Digital Pin 2
- Moisture Sensor: Analog Pin A0
- LCD: I2C connection (SDA/SCL pins)

## Setup Instructions

1. Clone this repository
2. Install required Arduino libraries through the Arduino IDE Library Manager
3. Create a `arduino_secrets.h` file with your WiFi and ThingSpeak credentials:
   ```cpp
   #define SECRET_SSID "your_wifi_ssid"
   #define SECRET_PASS "your_wifi_password"
   #define SECRET_WRITE_APIKEY "your_thingspeak_write_api"
   #define SECRET_CH_ID your_channel_id
   ```
4. Connect the hardware components according to the pin configuration
5. Upload the code to your Arduino Uno R4 WiFi

## Operation

The system performs the following operations:

- Continuously monitors environmental conditions (temperature, humidity, light, soil moisture)
- Displays real-time sensor data on the LCD screen
- Automatically waters the plant when soil moisture falls below set threshold
- Sends data to ThingSpeak every 15 minutes for remote monitoring
- Implements smart watering cycles with absorption time between cycles

## ThingSpeak Integration

The system sends the following data to ThingSpeak:
1. Temperature (°C)
2. Humidity (%)
3. Photoperiod (Day/Night)
4. Soil Moisture (%)
5. Raw Soil Moisture Value
6. Water Pump Cycles Count

## Calibration

The system includes calibrated values for:
- Soil Moisture Sensor:
  - Water Value: 1000
  - Air Value: 330
  - Moisture Threshold: 45%
- Light Sensor: Binary output (0 = day, 1 = night)

## Contributing

Feel free to submit issues, fork the repository, and create pull requests for any improvements.

## License

This project is open source and available under the [MIT License](LICENSE).

## Last Update

Last modified: January 7, 2025
