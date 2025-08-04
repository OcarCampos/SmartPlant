# SmartPlant - Automated Garden Monitoring System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)

An Arduino-based garden monitoring and automated watering system. This project uses various sensors to monitor plant health and environmental conditions, with data visualization through ThingSpeak and a smart automated watering functionality.

## Table of Contents

- [Features](#features)
- [Project Status](#project-status)
- [Hardware and Software](#hardware-and-software)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Dependencies](#software-dependencies)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Hardware Setup](#hardware-setup)
  - [Pin Configuration](#pin-configuration)
  - [Wiring Diagrams](#wiring-diagrams)
- [How It Works](#how-it-works)
  - [Sensor Monitoring](#sensor-monitoring)
  - [Smart Watering Logic](#smart-watering-logic)
  - [ThingSpeak Integration](#thingspeak-integration)
- [Project Structure](#project-structure)
- [Development and Testing](#development-and-testing)
- [3D Printable Cases](#3d-printable-cases)
- [Contributing](#contributing)
- [License](#license)

## Features

- **Environment Monitoring**: Real-time tracking of temperature, humidity, soil moisture, and light conditions.
- **Automated Watering**: Smart watering system that activates a water pump based on soil moisture levels.
- **Data Display**: Live sensor data shown on a 16x2 LCD screen.
- **Remote Monitoring**: WiFi connectivity to send data to the ThingSpeak platform for visualization and analysis.
- **Smart Watering Cycles**: Implements timed watering cycles with absorption periods to prevent overwatering.

## Project Status

This project is considered complete and functional. All features have been implemented and tested.

## Hardware and Software

### Hardware Requirements

| Component                      | Purpose                                    |
| ------------------------------ | ------------------------------------------ |
| Arduino Uno R4 WiFi            | Microcontroller with built-in WiFi         |
| DHT11 Sensor                   | Measures temperature and humidity          |
| LDR (Light Dependent Resistor) | Detects light levels (Day/Night)           |
| Custom Soil Moisture Sensor    | Measures soil moisture content             |
| 16x2 LCD Display with I2C      | Displays real-time sensor data             |
| Relay Module                   | Switches the high-voltage water pump       |
| Water Pump                     | Waters the plant                           |
| Power Supply                   | Provides power to the system               |
| Jumper Wires & Breadboard      | Connects all components                    |

### Software Dependencies

The following Arduino libraries are required. You can install them through the Arduino IDE Library Manager.

- `WiFiS3`
- `Wire`
- `LiquidCrystal_I2C`
- `DHT`
- `ThingSpeak`

## Getting Started

### Prerequisites

- Arduino IDE installed on your computer.
- A ThingSpeak account for data visualization.

### Installation

1.  **Clone the repository:**
    ```sh
    git clone https://github.com/your-username/SmartPlant.git
    ```
2.  **Install Libraries:** Open the Arduino IDE and install the libraries listed under [Software Dependencies](#software-dependencies).
3.  **Configure Secrets:** Create a header file named `arduino_secrets.h` in the main project directory with your WiFi and ThingSpeak credentials:
    ```cpp
    #define SECRET_SSID "your_wifi_ssid"
    #define SECRET_PASS "your_wifi_password"
    #define SECRET_CH_ID your_thingspeak_channel_id
    #define SECRET_WRITE_APIKEY "your_thingspeak_write_api_key"
    ```
4.  **Hardware Assembly:** Connect the hardware components according to the [Pin Configuration](#pin-configuration) and [Wiring Diagrams](#wiring-diagrams).
5.  **Upload the Code:** Open `SmartPlant.ino` in the Arduino IDE, select the correct board and port, and upload the sketch.

## Hardware Setup

### Pin Configuration

| Component           | Pin on Arduino Uno R4 | Pin Type |
| ------------------- | --------------------- | -------- |
| DHT11 Sensor        | Digital Pin 8         | Digital  |
| Light Sensor (LDR)  | Digital Pin 9         | Digital  |
| Relay Module        | Digital Pin 2         | Digital  |
| Soil Moisture Sensor| Analog Pin A0         | Analog   |
| LCD Display (I2C)   | SDA / SCL             | I2C      |

### Wiring Diagrams

The `Diagrams` folder contains detailed Fritzing diagrams for connecting each component to the Arduino board. These visual guides simplify the hardware setup process.

## How It Works

### Sensor Monitoring

The system continuously reads data from the sensors:
- **DHT11**: Measures air temperature (°C) and humidity (%).
- **LDR**: Determines if it's day or night.
- **Soil Moisture Sensor**: Provides an analog value representing the soil's moisture level, which is then converted to a percentage.

### Smart Watering Logic

The watering process is triggered if the soil moisture drops below a predefined threshold (`45%`):

1.  The system initiates a **3-cycle watering process** to ensure proper soil absorption.
2.  **Watering Cycle**: The water pump runs for 5 seconds.
3.  **Absorption Pause**: The pump stops for 20 seconds to allow the water to seep into the soil.
4.  This cycle repeats three times.

The number of watering sessions is tracked and sent to ThingSpeak.

### ThingSpeak Integration

Every 15 minutes, the collected data is sent to a ThingSpeak channel for remote monitoring. The following data fields are used:

- **Field 1**: Temperature (°C)
- **Field 2**: Humidity (%)
- **Field 3**: Photoperiod (0 for Day, 1 for Night)
- **Field 4**: Soil Moisture (%)
- **Field 5**: Raw Soil Moisture Value
- **Field 6**: Water Pump Cycle Count

## Project Structure

```
.SmartPlant/
├── 3D-Cases/ # STL files for 3D printable cases
├── DevModules/ # Scripts for calibration and testing
├── Diagrams/ # Fritzing wiring diagrams
├── SmartPlant.ino # Main Arduino sketch
├── arduino_secrets.h.example # Example secrets file
└── README.md # This file
```

## Development and Testing

The `DevModules` directory contains various scripts for calibrating sensors, testing hardware components, and verifying connectivity. These are invaluable for initial setup and troubleshooting.

## 3D Printable Cases

The `3D-Cases` folder contains STL files for custom enclosures for the project's components, sourced from [Printables](https://www.printables.com). Using these cases provides protection and a clean, professional look to the final assembly.

## Contributing

Contributions are welcome! Feel free to submit issues, fork the repository, and create pull requests for any improvements.

## License

This project is open source and available under the [MIT License](LICENSE).
