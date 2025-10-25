// ============================================================================
// Project Title : ESP32 Environment Monitor with DHT11, LDR & OLED Display
// Name          : Muhammad Umer
// Registration No : 23-NTU-CS-1078
// Section      : BSCS-5th-B
//
// Description:
// This program reads data from a DHT11 temperature/humidity sensor and an LDR
// (Light Dependent Resistor) using the ESP32’s ADC pin. The collected data
// (Temperature, Humidity, and Light Voltage) is displayed on an OLED screen
// and printed to the Serial Monitor for debugging.
//
// Hardware Connections:
//   - DHT11 Sensor Data → GPIO 14
//   - LDR Sensor (via voltage divider) → GPIO 34 (Analog Input)
//   - OLED Display SDA → GPIO 21, SCL → GPIO 22
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---------------------- Pin Configuration ----------------------
#define DHTPIN 14          // DHT11 data pin connected to GPIO14
#define DHTTYPE DHT11      // Specify sensor type (DHT11)
#define LDR_PIN 34         // LDR analog input pin

#define SDA_PIN 21         // I2C SDA pin for OLED
#define SCL_PIN 22         // I2C SCL pin for OLED

// ---------------------- OLED Display Setup ----------------------
#define SCREEN_WIDTH 128   // OLED width in pixels
#define SCREEN_HEIGHT 64   // OLED height in pixels

// Create display object using I2C communication
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------------- Sensor Object ---------------------------
DHT dht(DHTPIN, DHTTYPE);  // Initialize DHT sensor

// ============================================================================
// Setup Function: Runs once during startup
// ============================================================================
void setup() {
  Serial.begin(115200);
  Serial.println("Combined Sensor Project Initializing...");

  // Initialize I2C communication for OLED
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);  // Stop program if OLED fails to initialize
  }

  // Display startup message
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
  delay(500);

  // Start DHT11 sensor
  dht.begin();
  delay(1000);
}

// ============================================================================
// Loop Function: Continuously reads sensors and updates display
// ============================================================================
void loop() {
  // ----------- Read Temperature and Humidity from DHT11 -----------
  float temperature = dht.readTemperature();   // °C
  float humidity = dht.readHumidity();         // %

  // ----------- Read LDR Analog Value -----------
  int adcValue = analogRead(LDR_PIN);          // Range: 0–4095 for ESP32 ADC
  float voltage = (adcValue / 4095.0) * 3.3;   // Convert ADC value to voltage

  // ----------- Print Data to Serial Monitor -----------
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error reading DHT11 sensor! (Skipping T/H)");
  } else {
    Serial.printf("Temp: %.1f °C | Humidity: %.1f %%\n", temperature, humidity);
  }
  Serial.printf("LDR ADC: %d | Voltage: %.2f V\n", adcValue, voltage);
  Serial.println("---------------------------------");

  // ----------- Display Data on OLED -----------
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Title
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Hello IoT");

  // Temperature & Humidity Section
  display.setTextSize(1);
  if (!isnan(temperature) && !isnan(humidity)) {
    display.setCursor(0, 16);
    display.print("Temp: ");
    display.print(temperature, 1);
    display.print((char)247);  // Degree symbol
    display.println("C");

    display.setCursor(0, 32);
    display.print("Humidity: ");
    display.print(humidity, 0);
    display.println(" %");
  } else {
    display.setCursor(0, 16);
    display.println("T/H Read Error!");
  }

  // LDR Section
  display.setTextSize(1);
  display.setCursor(0, 54);
  display.print("LDR:");
  display.print(adcValue);
  display.print(" / ");
  display.print(voltage, 2);
  display.println("V");

  // Update OLED
  display.display();

  // Delay before next reading
  delay(2000);
}
