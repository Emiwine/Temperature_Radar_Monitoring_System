# IoT Device Monitoring and Control System

This project implements an IoT-based monitoring and control system, leveraging sensors and MQTT for data communication and device management. It provides features for temperature and presence monitoring, firmware updates via OTA, and MQTT-based device control.

## Key Features

### 1. **Temperature Monitoring**
- Utilizes the AHT10/AHT20 sensor to measure temperature.
- Publishes temperature data to an MQTT topic (`temperature/{deviceId}`).
- Implements error handling for corrupted sensor data.

### 2. **Presence Detection**
- Employs the LD2410 radar sensor to detect moving and stationary targets.
- Publishes presence data, including distance and energy levels, to an MQTT topic (`presence/{deviceId}`).
- Includes mechanisms to handle scenarios with no detected targets and send alerts.

### 3. **MQTT Integration**
- Connects to an MQTT broker for real-time data communication.
- Subscribes to topics for receiving commands (e.g., `enquireip`, `restart`, `ota`).
- Publishes sensor data and alerts for temperature and presence.

### 4. **Wi-Fi and Hotspot Handling**
- Automatically connects to a predefined Wi-Fi network.
- Switches to hotspot mode if Wi-Fi connection fails.

### 5. **Over-The-Air (OTA) Updates**
- Enables remote firmware updates through specified URLs.
- Verifies and updates firmware seamlessly.

### 6. **Error Handling and Alerts**
- Detects and reports corrupted data or device misconfigurations.
- Publishes alerts to dedicated MQTT topics (`TemperatureAlert/` and `PresenceAlert/`).

## Code Highlights

### **Main Code Workflow** (`main.cpp`)
- Initializes all components, including EEPROM, sensors, MQTT, and OTA.
- Continuously monitors Wi-Fi status and publishes sensor data every 5 seconds.
- Listens for MQTT commands and handles device-specific tasks such as firmware updates.

### **Temperature Sensor Initialization and Data Handling** (`deviceControl.cpp`)
- Configures the AHT10/AHT20 sensor.
- Provides real-time temperature readings.
- Manages sensor errors and ensures reliable data.

### **Presence Sensor Initialization and Data Handling** (`deviceControl.cpp`)
- Sets up the LD2410 radar sensor for presence detection.
- Processes and publishes presence data, including stationary and moving targets.

### **MQTT Implementation** (`Mqtt.cpp`)
- Configures MQTT client and connects to the broker.
- Manages topic subscriptions and message callbacks.
- Publishes temperature and presence data.
- Handles device commands received through MQTT.

## Project Setup

1. **Hardware Requirements**
   - ESP32/ESP8266 microcontroller.
   - AHT10/AHT20 temperature sensor.
   - LD2410 radar presence sensor.

2. **Software Requirements**
   - Arduino IDE with necessary libraries (e.g., `Adafruit_AHTX0`, `PubSubClient`).
   - MQTT broker (e.g., EMQX).

3. **Steps to Run the Project**
   - Clone the repository and open the project in Arduino IDE.
   - Update Wi-Fi and MQTT credentials in the code.
   - Upload the code to the microcontroller.
   - Monitor the serial output for logs and data.

## Key MQTT Topics

| Topic                  | Description                                  |
|------------------------|----------------------------------------------|
| `temperature/{deviceId}` | Publishes temperature data.                 |
| `presence/{deviceId}`    | Publishes presence detection data.          |
| `alert/{deviceId}`       | Receives device-specific alerts or commands.|
| `TemperatureAlert/{deviceId}` | Publishes corrupted temperature data alerts. |
| `PresenceAlert/{deviceId}` | Publishes no target detected alerts.         |

## Future Enhancements
- Add support for additional sensors and actuators.
- Improve error recovery mechanisms.
- Expand MQTT topic structure for better scalability.

This system is a scalable and modular solution for IoT-based device monitoring and control, ensuring reliable performance and ease of integration with existing IoT ecosystems.

