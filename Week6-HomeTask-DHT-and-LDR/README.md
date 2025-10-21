# Home-Task (Week 6): ESP32 Environmental Monitor

***This project combines two tasks to create a single, unified environmental monitoring system using an **ESP32 microcontroller**.* It simultaneously reads data from a **DHT11** Temperature/Humidity sensor and an LDR (Light-Dependent Resistor), displaying all results in real-time on an **SSD1306 OLED screen***.

*The primary display output is **"Hello IoT"**.*

## 1. Hardware and Software Setup

### 1.1 Components Used

* **Microcontroller:** *ESP32 Development Board*

* **Temperature/Humidity:** ***DHT11*** *Sensor*

* **Light Intensity:** *LDR (Photoresistor)*

* **Display:** *SSD1306 128x64 I2C OLED Display*

* **Other:** *Resistor (for LDR circuit), Breadboard, Jumper Wires*

### 1.2 Pin Configuration

*Pin assignments are: **OLED** (SDA: *21*, SCL: *22*), **DHT11 Data** (*14*), and **LDR Analog** (*34*).* *The LDR is connected in a voltage divider, with the analog output taken from the junction between the LDR and the* $10\text{k}\Omega$ *resistor.*

## 2. Source Code (`main.cpp`) Overview

*The **C++ code** updates the display every **2 seconds**.* *The **setup()** initializes the Serial Monitor, I2C pins, OLED (displaying "**Initializing...**"), and the DHT sensor.*

*The **loop()** function reads the DHT11 (Temp/Humidity) and LDR (raw ADC and calculated voltage).* *The data is then presented on the OLED under the title "**Hello IoT**," with readings formatted for **Temp**, **Humidity**, and a combined **LDR: ADC / Voltage** output.*

## 3. Screenshots of Working Design

### 3.1 Full Circuit and General Operation

![IMG_2061](https://github.com/user-attachments/assets/74283606-fb77-4675-b5f9-0bcd6b346bb4)


**Description:** *This image confirms the correct component integration and wiring of the **ESP32**, sensors, and **OLED display**.*

### 3.2 Sensor Readings Under Normal Light

![IMG_2065](https://github.com/user-attachments/assets/404cc8d7-63ba-4fb6-9d9f-b5e5c28451bc)


**Description:** *This screenshot shows the **baseline readings**: Temp:* $27.6^{\circ}\text{C}$*, Humidity:* $51\%$*, LDR ADC/Voltage:* $457 / 0.37\text{V}$.\*

### 3.3 Sensor Readings Under Low Light (LDR Covered)

![IMG_2066](https://github.com/user-attachments/assets/8a428c7e-b70a-4c7d-85c6-9486c58d5ff5)


**Description:** *This screenshot demonstrates the **LDR's quick response** when blocked, with the reading dropping significantly* (LDR: $0 / 0.00\text{V}$).

## 4. Sensor Analysis and Observations

***The following analysis details the reactions of the sensors to intentional environmental changes.***

| **Sensor** | **Action Taken** | **Observed Change** | **Analysis** | 
| ----- | ----- | ----- | ----- | 
| **LDR** | Covered sensor. | ADC/Voltage dropped to **0** (or near-zero). | *Increased resistance in darkness pulls the analog voltage down.* | 
| **LDR** | Shined a phone flashlight. | ADC/Voltage rose sharply. | *Decreased resistance in high light allows the analog voltage to rise.* | 
| **DHT11 (Temp)** | Gently exhaled warm air. | Temperature showed an immediate **increase**. | *Confirms sensor response to convective heat transfer.* | 
| **DHT11 (Humidity)** | Gently exhaled moist air. | Humidity reading showed a **spike**. | *The capacitive element quickly detected the change in air moisture.* | 

***The combined setup successfully provides **real-time, multi-sensor environmental data** on a single, compact display.***
