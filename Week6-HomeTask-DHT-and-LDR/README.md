# 🌡️ ESP32 Environment Monitor (DHT11 + LDR + OLED)

### 📘 Overview
This project demonstrates how to build a **simple IoT environment monitoring system** using the **ESP32** microcontroller.  
It reads **temperature**, **humidity**, and **light intensity** values from sensors and displays them on an **OLED screen**.

---

### 🧠 Features
- Reads **temperature & humidity** via DHT11 sensor  
- Measures **ambient light** using an LDR (Light Dependent Resistor)  
- Displays live readings on a **0.96” OLED (SSD1306)**  
- Prints data on **Serial Monitor** for debugging  
- Uses **I2C communication** for display connection  

---

### 🧰 Hardware Components
| Component | Quantity | Description |
|------------|-----------|-------------|
| ESP32 Dev Module | 1 | Main microcontroller |
| DHT11 Sensor | 1 | Measures temperature and humidity |
| LDR | 1 | Detects light intensity |
| Resistor (10kΩ) | 1 | Used in voltage divider with LDR |
| OLED Display (SSD1306) | 1 | 0.96" I2C screen for output |
| Jumper Wires | Several | For connections |
| Breadboard | 1 | For easy prototyping |

---

### ⚙️ Circuit Connections

| Component | ESP32 Pin |
|------------|------------|
| DHT11 Signal | GPIO14 |
| LDR | GPIO34 (Analog Input) |
| OLED SDA | GPIO21 |
| OLED SCL | GPIO22 |
| VCC | 3.3V |
| GND | GND |

**Note:** LDR is connected in a voltage divider with a 220Ω resistor.

---

### 📸 Hardware Setup

| View | Image |
|------|--------|
| Complete Setup | ![Setup](/screenshots/IMG_2061.JPG) |
| OLED Close-Up (Bright Light) | ![OLED Bright](/screenshots/IMG_2065.JPG) |
| OLED Close-Up (Low Light) | ![OLED Dim](/screenshots/IMG_2066.JPG) |

---

### 🧩 Working Principle
1. The **DHT11** sensor reads **temperature** and **humidity** values.
2. The **LDR** detects **light intensity** by converting brightness into resistance, read via ADC.
3. The **ESP32** processes and displays the data on the **OLED** and **Serial Monitor**.
4. The voltage from the LDR is calculated to understand light level changes.

---

### 🧑‍💻 Libraries Required
Install these libraries from **Arduino Library Manager**:
- `Adafruit SSD1306`
- `Adafruit GFX`
- `DHT sensor library`

---

### 🚀 Future Enhancements
- Add **WiFi + MQTT** support to upload data to a cloud dashboard  
- Use **BME280 sensor** for more accurate readings  
- Include **auto brightness adjustment** for OLED display  

---

**Author:** Muhammad Umer 
**Registration No:** 23-NTU-CS-1078  
**Platform:** Arduino Framework (ESP32 Dev Module)
