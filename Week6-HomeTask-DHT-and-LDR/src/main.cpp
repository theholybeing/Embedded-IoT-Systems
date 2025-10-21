#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define DHTPIN 14
#define DHTTYPE DHT11
#define LDR_PIN 34

#define SDA_PIN 21
#define SCL_PIN 22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("Combined Sensor Project Initializing...");

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
  delay(500);

  dht.begin();
  delay(1000);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int adcValue = analogRead(LDR_PIN);
  float voltage = (adcValue / 4095.0) * 3.3;

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error reading DHT11 sensor! (Skipping T/H)");
  } else {
    Serial.printf("Temp: %.1f °C | Humidity: %.1f %%\n", temperature, humidity);
  }
  Serial.printf("LDR ADC: %d | Voltage: %.2f V\n", adcValue, voltage);
  Serial.println("---------------------------------");

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Hello IoT");

  display.setTextSize(1);
  if (!isnan(temperature) && !isnan(humidity)) {
    display.setCursor(0, 16);
    display.print("Temp: ");
    display.print(temperature, 1);
    display.print((char)247);
    display.println("C");

    display.setCursor(0, 32);
    display.print("Humidity: ");
    display.print(humidity, 0);
    display.println(" %");
  } else {
    display.setCursor(0, 16);
    display.println("T/H Read Error!");
  }

  display.setTextSize(1);
  display.setCursor(0, 54);
  display.print("LDR:");
  display.print(adcValue);
  display.print(" / ");
  display.print(voltage, 2);
  display.println("V");

  display.display();

  delay(2000);
}