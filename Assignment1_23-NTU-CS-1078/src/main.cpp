// ============================================================================
// Project Title : ESP32 Smart LED Controller with OLED Display & Buzzer
// Name          : Muhammad Umer
// Reg. No       : 23-NTU-CS-1078
//
// Description:
// This firmware controls 3 LEDs and a buzzer using an ESP32. 
// It demonstrates multiple lighting patterns, button-based interactions,
// and OLED feedback for visual monitoring.
//
// ▶ Mode Button → Switches between LED effects:
//     1. OFF → 2. ALTERNATE → 3. ALL ON → 4. SMOOTH PWM FADE
// ▶ Action Button → Short press toggles all LEDs; long press activates buzzer
// ▶ Boot Button  → Resets everything back to OFF state
//
// Components Used:
//   - ESP32 NodeMCU (DevKit v1)
//   - 3 LEDs (Red, Green, Yellow)
//   - 2 Push Buttons (Mode, Action)
//   - 1 Piezo Buzzer
//   - 0.96" I2C OLED Display (SSD1306)
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// ----------------------- Pin Mapping -----------------------
#define PIN_BUZZER      27
#define PIN_MODE_BTN    13
#define PIN_ACTION_BTN  12
#define PIN_BOOT_BTN    0    // Built-in boot/reset button

#define PIN_LED_RED     19
#define PIN_LED_GREEN   18
#define PIN_LED_YELLOW  5

// ----------------------- PWM Configuration -----------------
#define PWM_RESOLUTION  8
#define LED_PWM_FREQ    5000
#define BUZZER_CHANNEL  0
#define RED_CHANNEL     1
#define GREEN_CHANNEL   2
#define YELLOW_CHANNEL  3

// ----------------------- OLED Setup ------------------------
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_ADDR   0x3C
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

// ----------------------- Mode Management -------------------
enum LightingMode {
  MODE_OFF = 0,
  MODE_ALTERNATE,
  MODE_ALL_ON,
  MODE_FADE,
  TOTAL_MODES
};

LightingMode currentMode = MODE_OFF;

// ----------------------- Timing Variables ------------------
unsigned long blinkTimer = 0;
const unsigned long BLINK_DELAY = 400;
bool blinkState = false;

unsigned long fadeTimer = 0;
unsigned long btnPressStart = 0;
bool btnPressed = false;
bool longPressHandled = false;

unsigned long lastModePress = 0, lastBootPress = 0;
const unsigned long DEBOUNCE_DELAY = 50;

// Manual LED toggle state (used after short press)
bool manualOverride = false;
bool manualLedState = false;

// ============================================================================
// OLED Utility — Print text on 2 lines (header + main info)
// ============================================================================
void showOLED(const char* header, const char* message) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println(header);

  oled.setTextSize(2);
  oled.setCursor(0, 20);
  oled.println(message);
  oled.display();
}

// ============================================================================
// Change LED Mode — Called when MODE button is pressed
// ============================================================================
void changeMode(LightingMode mode) {
  currentMode = mode;
  manualOverride = false;  // disable manual LED state if new mode selected

  switch (mode) {
    case MODE_OFF:
      showOLED("Mode:", "All OFF");
      ledcWrite(RED_CHANNEL, 0);
      ledcWrite(GREEN_CHANNEL, 0);
      ledcWrite(YELLOW_CHANNEL, 0);
      break;

    case MODE_ALTERNATE:
      showOLED("Mode:", "Alternate");
      blinkTimer = millis();
      blinkState = false;
      break;

    case MODE_ALL_ON:
      showOLED("Mode:", "All ON");
      ledcWrite(RED_CHANNEL, 255);
      ledcWrite(GREEN_CHANNEL, 255);
      ledcWrite(YELLOW_CHANNEL, 255);
      break;

    case MODE_FADE:
      showOLED("Mode:", "PWM Fade");
      fadeTimer = millis();
      break;
  }
}

// ============================================================================
// Buzzer Utility — Simple tone function
// ============================================================================
void playTone(unsigned int frequency, unsigned long duration) {
  ledcWriteTone(BUZZER_CHANNEL, frequency);
  delay(duration);
  ledcWriteTone(BUZZER_CHANNEL, 0);
}

// ============================================================================
// SETUP — Initialization Code
// ============================================================================
void setup() {
  Serial.begin(115200);

  // Configure input pins with internal pull-ups
  pinMode(PIN_MODE_BTN, INPUT_PULLUP);
  pinMode(PIN_ACTION_BTN, INPUT_PULLUP);
  pinMode(PIN_BOOT_BTN, INPUT_PULLUP);

  // Configure LED pins for PWM output
  ledcSetup(BUZZER_CHANNEL, 2000, PWM_RESOLUTION);
  ledcAttachPin(PIN_BUZZER, BUZZER_CHANNEL);

  ledcSetup(RED_CHANNEL, LED_PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PIN_LED_RED, RED_CHANNEL);

  ledcSetup(GREEN_CHANNEL, LED_PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PIN_LED_GREEN, GREEN_CHANNEL);

  ledcSetup(YELLOW_CHANNEL, LED_PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PIN_LED_YELLOW, YELLOW_CHANNEL);

  // Initialize OLED Display
  Wire.begin(21, 22);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true) delay(100); // stop execution if OLED not detected
  }

  showOLED("System:", "Ready");
  changeMode(MODE_OFF);
}

// ============================================================================
// LOOP — Core Logic
// ============================================================================
void loop() {
  unsigned long now = millis();

  // -------------------- MODE BUTTON --------------------
  if (digitalRead(PIN_MODE_BTN) == LOW) {
    if (now - lastModePress > DEBOUNCE_DELAY) {
      currentMode = (LightingMode)((currentMode + 1) % TOTAL_MODES);
      changeMode(currentMode);
      while (digitalRead(PIN_MODE_BTN) == LOW) delay(10);
      lastModePress = millis();
    }
  }

  // -------------------- BOOT BUTTON --------------------
  if (digitalRead(PIN_BOOT_BTN) == LOW) {
    if (now - lastBootPress > DEBOUNCE_DELAY) {
      changeMode(MODE_OFF);
      showOLED("System:", "Reset (BOOT)");
      while (digitalRead(PIN_BOOT_BTN) == LOW) delay(10);
      lastBootPress = millis();
    }
  }

  // -------------------- ACTION BUTTON --------------------
  int actionState = digitalRead(PIN_ACTION_BTN);

  // Detect initial press
  if (actionState == LOW && !btnPressed) {
    btnPressed = true;
    btnPressStart = now;
    longPressHandled = false;
  }

  // Handle long press (buzzer)
  if (actionState == LOW && btnPressed && !longPressHandled) {
    if (now - btnPressStart >= 1500) { // 1.5 sec = long press
      showOLED("Action:", "Long Press");
      playTone(2500, 300);
      longPressHandled = true;
    }
  }

  // Handle release
  if (actionState == HIGH && btnPressed) {
    unsigned long pressDuration = now - btnPressStart;
    btnPressed = false;

    // If short press, toggle LEDs manually
    if (!longPressHandled && pressDuration < 1500) {
      manualOverride = true;
      manualLedState = !manualLedState;

      if (manualLedState) {
        ledcWrite(RED_CHANNEL, 255);
        ledcWrite(GREEN_CHANNEL, 255);
        ledcWrite(YELLOW_CHANNEL, 255);
        showOLED("Action:", "Short: ON");
      } else {
        ledcWrite(RED_CHANNEL, 0);
        ledcWrite(GREEN_CHANNEL, 0);
        ledcWrite(YELLOW_CHANNEL, 0);
        showOLED("Action:", "Short: OFF");
      }
    }
  }

  // -------------------- LED MODE BEHAVIOR --------------------
  if (!manualOverride) {
    switch (currentMode) {

      case MODE_ALTERNATE:
        if (now - blinkTimer >= BLINK_DELAY) {
          blinkTimer = now;
          blinkState = !blinkState;
          static int ledIndex = 0;
          if (blinkState) {
            ledIndex = (ledIndex + 1) % 3;
            ledcWrite(RED_CHANNEL, (ledIndex == 0) ? 255 : 0);
            ledcWrite(GREEN_CHANNEL, (ledIndex == 1) ? 255 : 0);
            ledcWrite(YELLOW_CHANNEL, (ledIndex == 2) ? 255 : 0);
          } else {
            ledcWrite(RED_CHANNEL, 0);
            ledcWrite(GREEN_CHANNEL, 0);
            ledcWrite(YELLOW_CHANNEL, 0);
          }
        }
        break;

      case MODE_FADE: {
        unsigned long elapsed = (now - fadeTimer) % 2000;
        float t = elapsed / 2000.0f;
        float r = (sin(2 * PI * t) + 1.0f) / 2.0f;
        float g = (sin(2 * PI * (t + 1.0 / 3.0)) + 1.0f) / 2.0f;
        float y = (sin(2 * PI * (t + 2.0 / 3.0)) + 1.0f) / 2.0f;

        ledcWrite(RED_CHANNEL,   (int)(r * 255));
        ledcWrite(GREEN_CHANNEL, (int)(g * 255));
        ledcWrite(YELLOW_CHANNEL,(int)(y * 255));
        break;
      }

      default:
        break;
    }
  }

  delay(8); // short delay for stability
}