# Smart Aquarium – ESP32 IoT Project 🐠

The **Smart Aquarium** is an ESP32-based IoT system designed to automate and monitor aquarium operations such as temperature control, water circulation, lighting, and fish feeding.  
This project was developed as part of an **Embedded IoT Systems** course and demonstrates real-world integration of sensors, actuators, displays, and cloud connectivity.

## 🔍 Project Overview

The system uses an **ESP32 microcontroller** to continuously monitor environmental conditions and control aquarium components automatically or remotely through an IoT interface.

Key goals of the project:
- Reduce manual aquarium maintenance  
- Ensure stable environmental conditions for aquatic life  
- Enable remote monitoring and control via Wi-Fi  

## ⚙️ Features

- 🌡 **Temperature Monitoring**
  - Uses a temperature sensor to monitor water temperature
  - Heater controlled automatically or manually

- 💧 **Water Pump Control**
  - Controls water circulation using a relay module

- 💡 **Lighting System**
  - LED lights controlled via software and IoT interface

- 🍽 **Automatic Fish Feeding**
  - Servo motor rotates to dispense food on command or schedule

- 📟 **OLED Display**
  - Displays real-time system status, time, and sensor readings

- 🌐 **IoT Connectivity**
  - ESP32 connects to Wi-Fi
  - Remote control and monitoring via cloud platform / web dashboard

## 🧠 System Components

### Hardware Used
- ESP32 Development Board  
- Temperature Sensor (DS18B20 / DHT)  
- Relay Module (for pump and heater)  
- Water Pump  
- Electric Heater  
- Servo Motor (Fish Feeder)  
- OLED Display (SSD1306)  
- LEDs  
- Power Supply  
- Jumper wires & breadboard  

### Software & Libraries
- Arduino IDE  
- ESP32 Board Package  
- Adafruit SSD1306 & GFX Libraries  
- Servo Library  
- WiFi / IoT libraries  

## 🛠 System Architecture

1. Sensors collect real-time aquarium data  
2. ESP32 processes the data  
3. Actuators (heater, pump, lights, feeder) are controlled based on logic or user commands  
4. OLED displays live status  
5. IoT platform enables remote access and control  

## ▶️ How to Run the Project

1. Clone the main repository:
   ```bash
   git clone https://github.com/theholybeing/Embedded-IoT-Systems.git
