# IoT-Based Music and LED Control System

This repository contains the code and documentation for an IoT-based system that uses an ESP32 microcontroller to control a passive buzzer and five LEDs via the MQTT protocol. The system allows remote control through a mobile application, enabling the playback of distinct melodies and synchronized LED animations triggered by three buttons. This project was developed as part of an academic assignment at Politecnico di Torino to demonstrate the application of IoT technologies in educational and entertainment contexts.

## Table of Contents

- Project Overview
- Hardware Requirements
- Software Requirements
- Installation
- Configuration
- Usage
- Code Structure
- Troubleshooting
- Future Improvements
- License
- Acknowledgements

## Project Overview

The project implements a remotely controlled system using the ESP32 microcontroller. The system plays 10-12 second melodies on a passive buzzer and synchronizes five LEDs with the music rhythm, controlled via MQTT messages sent from a mobile application. The ESP32 connects to a Wi-Fi network and an MQTT broker (e.g., broker.emqx.io) to receive commands ("button1", "button2", "button3") from the mobile app, triggering different melodies and LED patterns (sequential, reverse sequential, or simultaneous).

The system architecture consists of:

- ESP32 Dev Module: Main control unit with Wi-Fi connectivity.
- Passive buzzer: Plays melodies at frequencies between 262 Hz and 880 Hz.
- Five LEDs: Connected to GPIO pins for visual animations.
- MQTT broker: Facilitates communication between the ESP32 and the mobile app.
- Mobile application: IoT MQTT Panel or similar, configured with three buttons.

## Hardware Requirements

- ESP32 Dev Module (e.g., ESP32-WROOM-32)
  - Dual-core Tensilica LX6 processor, 240 MHz
  - 520 KB SRAM, 4 MB flash memory
  - Wi-Fi (802.11 b/g/n), Bluetooth 4.2
- Passive buzzer
  - Operating voltage: 3.3V-5V
  - Frequency range: 100 Hz to 10 kHz
- Five LEDs (5 mm, red/green/blue)
  - Operating voltage: 2.0-3.3V
  - Current: 20 mA (with resistors)
- Resistors (220-330 Ohm, 1/4 watt)
- Breadboard (400-point)
- Jumper wires (male-to-male, male-to-female)
- USB cable (USB-A to Micro-USB, 1 meter)
- 2.4 GHz Wi-Fi network (with SSID and password)

### Circuit Diagram

The hardware is connected as follows:

- Buzzer: Connected to GPIO 25
- LEDs: Connected to GPIO 26, 27, 14, 12, 13, each with a 220-330 Ohm resistor
- ESP32: Powered via USB cable

\
*Note: Create the circuit diagram using Fritzing and upload it to the repository as* `circuit.png`*.*

## Software Requirements

- Arduino IDE (version 2.0 or higher)
- Libraries:
  - `WiFi.h` (included with ESP32 board package)
  - `PubSubClient.h` (install via Arduino Library Manager)
- ESP32 board package for Arduino IDE
- MQTT broker: Public broker (e.g., broker.emqx.io, port 1883)
- Mobile application: IoT MQTT Panel (available on Android/iOS) or any MQTT client

## Installation

1. **Set up the Arduino IDE**:

   - Download and install the Arduino IDE from arduino.cc.
   - Add the ESP32 board package:
     - Go to `File > Preferences`.
     - Add `https://dl.espressif.com/dl/package_esp32_index.json` to the "Additional Boards Manager URLs".
     - Go to `Tools > Board > Boards Manager`, search for "ESP32", and install the package by Espressif Systems.
   - Select the board: `Tools > Board > ESP32 Arduino > ESP32 Dev Module`.

2. **Install libraries**:

   - Open the Arduino IDE.
   - Go to `Sketch > Include Library > Manage Libraries`.
   - Search for and install `PubSubClient` by Nick O'Leary.

3. **Clone the repository**:

   ```bash
   git clone https://github.com/[your-username]/[your-repo-name].git
   ```

   - Open the `main.ino` file in the Arduino IDE.

4. **Assemble the hardware**:

   - Connect the ESP32, buzzer, LEDs, and resistors on a breadboard as shown in the circuit diagram.
   - Ensure all connections are secure and resistors are used to protect LEDs.

5. **Upload the code**:

   - Connect the ESP32 to your computer via the USB cable.
   - In the Arduino IDE, select the correct port: `Tools > Port`.
   - Click `Upload` to compile and upload the code to the ESP32.

## Configuration

1. **Wi-Fi settings**:

   - Open `main.ino` and update the following lines with your Wi-Fi credentials:

     ```cpp
     const char* ssid = "YOUR_WIFI_SSID";
     const char* password = "YOUR_WIFI_PASSWORD";
     ```

   - Ensure your Wi-Fi network operates on 2.4 GHz (ESP32 does not support 5 GHz).

2. **MQTT settings**:

   - The code uses the public MQTT broker `broker.emqx.io` (port 1883). No changes are needed unless you want to use a different broker.

   - The topic is set to `esp32/buttons`, and the Client ID is `ESP32Client-UniqueID123`. Modify these if necessary:

     ```cpp
     const char* mqtt_server = "broker.emqx.io";
     const char* mqtt_topic = "esp32/buttons";
     const char* client_id = "ESP32Client-UniqueID123";
     ```

3. **Mobile application setup**:

   - Install IoT MQTT Panel (or another MQTT client) on your smartphone.
   - Configure the app:
     - Broker: `broker.emqx.io`, port 1883
     - Topic: `esp32/buttons`
     - Add three buttons with payloads: `button1`, `button2`, `button3`
   - Save the configuration and connect to the broker.

## Usage

1. Power the ESP32 via the USB cable.
2. Open the Serial Monitor in the Arduino IDE (`Tools > Serial Monitor`, 115200 baud) to monitor Wi-Fi and MQTT connection status.
3. Wait for the ESP32 to connect to Wi-Fi and the MQTT broker (indicated by "Connected to Wi-Fi!" and "Connected to MQTT!" messages).
4. Open the mobile application and press one of the three buttons:
   - Button 1: Plays melody1 (10-12 seconds) with sequential LED flashing.
   - Button 2: Plays melody2 with reverse sequential LED flashing.
   - Button 3: Plays melody3 with simultaneous LED flashing.
5. Observe the buzzer playing the melody and the LEDs animating in sync with the music rhythm.

## Code Structure

The main code (`main.ino`) includes the following key functions:

- `setup()`: Initializes GPIO pins, connects to Wi-Fi, and sets up MQTT.
- `loop()`: Monitors Wi-Fi and MQTT connections, processes incoming messages.
- `callback()`: Handles MQTT messages and triggers melodies/LEDs.
- `playMelody()`: Plays melodies and synchronizes LED animations.
- `reconnect()`: Re-establishes Wi-Fi or MQTT connections if lost.

Key features:

- Three melodies (melody1, melody2, melody3) with 15 notes each, ranging from 262 Hz to 880 Hz.
- LED animations using PWM for dynamic brightness based on note frequencies.
- Robust error handling for Wi-Fi and MQTT connectivity.

## Troubleshooting

- **Wi-Fi connection failure**:
  - Check SSID and password accuracy.
  - Ensure the router is on a 2.4 GHz network.
  - Verify signal strength (test within 5-10 meters of the router).
  - Serial Monitor may show errors like `WL_NO_SSID_AVAIL` or `WL_CONNECT_FAILED`.
- **MQTT connection errors**:
  - Error code `rc=-4`: Connection timeout; check broker availability.
  - Error code `rc=2`: Invalid Client ID; ensure the Client ID is unique.
  - Use an alternative broker (e.g., test.mosquitto.org) if needed.
- **Buzzer not playing**:
  - Verify the buzzer is connected to GPIO 25.
  - Check for loose connections or a faulty buzzer.
- **LEDs not flashing**:
  - Ensure LEDs are connected to GPIO 26, 27, 14, 12, 13 with resistors.
  - Test LEDs individually using a simple Arduino sketch.

## Future Improvements

- Add RGB LEDs for colorful animations.
- Integrate physical buttons for standalone control.
- Replace MQTT with the Blynk platform for a user-friendly interface.
- Implement Deep Sleep mode to reduce power consumption.
- Use an active buzzer or MP3 module for higher-quality audio.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements

- Politecnico di Torino for the academic support.
- Espressif Systems for the ESP32 documentation.
- Arduino community for libraries and tutorials.

EMQX for providing a free public MQTT broker.