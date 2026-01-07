//------------Semester Project: Smart Aquarium Controller------------//
//Group Members:
//- Muhammad Hassan Baig (23-NTU-CS-1070)
//- Muhammad Umer        (23-NTU-CS-1078)
//- Muhammad Naveed      (22-NTU-CS-1205)
// This code manages an aquarium's pump, heater, LED lighting, and fish feeder
// It uses Blynk for remote control and MQTT for state reporting



#define BLYNK_TEMPLATE_ID "TMPL6Tc8YLoAK"
#define BLYNK_TEMPLATE_NAME "Smart Aquarium"
#define BLYNK_AUTH_TOKEN "thMQXGtDYGgplCN-k8WAdDwoXn6xTZ_H"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RtcDS1302.h>

/************ WIFI & MQTT ************/
const char* ssid = "23-1078";
const char* password = "";
const char* mqtt_server = "192.168.43.68"; 
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

/************ PINS ************/
#define PUMP_RELAY   16
#define HEATER_RELAY 17
#define SERVO_PIN    4
#define LED_PIN      23

#define RTC_CLK 25
#define RTC_DAT 26
#define RTC_RST 27

#define OLED_SDA 21
#define OLED_SCL 22

/************ OBJECTS ************/
Servo feederServo;
ThreeWire rtcWire(RTC_DAT, RTC_CLK, RTC_RST);
RtcDS1302<ThreeWire> Rtc(rtcWire);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

/************ STATE VARIABLES ************/
bool pumpState = false;
bool heaterState = false;
bool ledState = false;
bool feedingNow = false;

// Override flags
bool pumpOverride = false;
bool heaterOverride = false;
bool ledOverride = false;

// Schedule Times (Start H, Start M, End H, End M)
// V10: Pump, V11: Heater, V12: LED
int pumpSH=0, pumpSM=0, pumpEH=0, pumpEM=0;
int heaterSH=0, heaterSM=0, heaterEH=0, heaterEM=0;
int ledSH=0, ledSM=0, ledEH=0, ledEM=0;

// Feeding Schedule
int feedH=8, feedM=0;
bool feedDoneToday = false;

// LED PWM
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

/************ HARDWARE CONTROL ************/

// Updates Hardware + MQTT + Blynk
void setPump(bool on, bool fromBlynk = false) {
    if (pumpState != on) {
        pumpState = on;
        digitalWrite(PUMP_RELAY, on ? LOW : HIGH); // Active LOW
        
        const char* pay = on ? "ON" : "OFF";
        client.publish("aquarium/state/pump", pay);
        
        if (!fromBlynk) {
            Blynk.virtualWrite(V0, on ? 1 : 0);
        }
    }
}

void setHeater(bool on, bool fromBlynk = false) {
    if (heaterState != on) {
        heaterState = on;
        digitalWrite(HEATER_RELAY, on ? LOW : HIGH); // Active LOW
        
        const char* pay = on ? "ON" : "OFF";
        client.publish("aquarium/state/heater", pay);
        
        if (!fromBlynk) {
            Blynk.virtualWrite(V1, on ? 1 : 0);
        }
    }
}

// PWM Fade Wrapper
void ledFade(bool fadeIn) {
    if (fadeIn) {
        ledcAttachPin(LED_PIN, ledChannel); // Attach before fading in
        // Fade 0 -> 255
        for (int i = 0; i <= 255; i += 5) {
            ledcWrite(ledChannel, i);
            delay(10);
        }
    } else {
        // Fade 255 -> 0
        for (int i = 255; i >= 0; i -= 5) {
            ledcWrite(ledChannel, i);
            delay(10);
        }
        ledcWrite(ledChannel, 0); // Ensure fully OFF
        ledcDetachPin(LED_PIN);   // Detach PWM
        digitalWrite(LED_PIN, LOW); // Hard pull-down
    }
}

void setLED(bool on, bool fromBlynk = false) {
    if (ledState != on) {
        ledState = on;
        
        ledFade(on);
        
        const char* pay = on ? "ON" : "OFF";
        client.publish("aquarium/state/led", pay);
        
        if (!fromBlynk) {
             Blynk.virtualWrite(V2, on ? 1 : 0);
        }
    }
}

void feedFish() {
    if (feedingNow) return;
    feedingNow = true;
    
    client.publish("aquarium/state/feed", "RUNNING");
    Blynk.virtualWrite(V3, 1);
    

    
    feederServo.attach(SERVO_PIN);
    
    // Rotate 0 to 180
    for(int i=0; i<=180; i+=2){ 
        feederServo.write(i); 
        delay(10); 
    }
    
    // Rotate 180 to 0
    for(int i=180; i>=0; i-=2){ 
        feederServo.write(i); 
        delay(10); 
    }
    
    feederServo.detach();
    
    feedingNow = false;
    client.publish("aquarium/state/feed", "IDLE");
    Blynk.virtualWrite(V3, 0); 
}

/************ BLYNK HANDLERS ************/
// V0: Pump Switch
BLYNK_WRITE(V0) {
    bool val = param.asInt();
    pumpOverride = true;
    setPump(val, true);
}
// V1: Heater Switch
BLYNK_WRITE(V1) {
    bool val = param.asInt();
    heaterOverride = true;
    setHeater(val, true);
}
// V2: LED Switch
BLYNK_WRITE(V2) {
    bool val = param.asInt();
    ledOverride = true;
    setLED(val, true);
}
// V3: Feed Button
BLYNK_WRITE(V3) {
    if(param.asInt() == 1) {
        feedFish();
    }
}

// Time Input Widgets parsing: Start(sec), Stop(sec), TZ...
// We just need Start/Stop
void parseTimeInput(const BlynkParam& param, int &sh, int &sm, int &eh, int &em) {
    long start = param[0].asLong();
    long stop = param[1].asLong();
    
    // TimeInput sends seconds from midnight
    sh = (start / 3600) % 24;
    sm = (start / 60) % 60;
    eh = (stop / 3600) % 24;
    em = (stop / 60) % 60;
}

BLYNK_WRITE(V10) { // Pump Schedule
    parseTimeInput(param, pumpSH, pumpSM, pumpEH, pumpEM);
    pumpOverride = false;
    // Also publish to MQTT for Node-RED visibility?
    char buf[20];
    sprintf(buf, "%02d:%02d-%02d:%02d", pumpSH, pumpSM, pumpEH, pumpEM);
    client.publish("aquarium/state/schedule/pump", buf);
}

BLYNK_WRITE(V11) { // Heater Schedule
    parseTimeInput(param, heaterSH, heaterSM, heaterEH, heaterEM);
    heaterOverride = false;
    char buf[20];
    sprintf(buf, "%02d:%02d-%02d:%02d", heaterSH, heaterSM, heaterEH, heaterEM);
    client.publish("aquarium/state/schedule/heater", buf);
}

BLYNK_WRITE(V12) { // LED Schedule
    parseTimeInput(param, ledSH, ledSM, ledEH, ledEM);
    ledOverride = false;
    char buf[20];
    sprintf(buf, "%02d:%02d-%02d:%02d", ledSH, ledSM, ledEH, ledEM);
    client.publish("aquarium/state/schedule/led", buf);
}


/************ MQTT CALLBACK ************/
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) msg += (char)payload[i];
    String t = String(topic);

    if (t == "aquarium/set/pump") {
        pumpOverride = true;
        setPump(msg == "ON", false);
    } 
    else if (t == "aquarium/set/heater") {
        heaterOverride = true;
        setHeater(msg == "ON", false);
    }
    else if (t == "aquarium/set/led") {
        ledOverride = true;
        setLED(msg == "ON", false);
    }
    else if (t == "aquarium/set/feed") {
        feedFish();
    }
    else if (t == "aquarium/set/override/reset") {
        pumpOverride = false;
        heaterOverride = false;
        ledOverride = false;
    }
}

void reconnectMqtt() {
    if (!client.connected()) {
        if (client.connect("ESP32Aquarium")) {
            client.subscribe("aquarium/set/#");
        }
    }
}

/************ SETUP ************/
void setup() {
    Serial.begin(115200);

    // Init Pins
    pinMode(PUMP_RELAY, OUTPUT);
    digitalWrite(PUMP_RELAY, HIGH); // OFF
    pinMode(HEATER_RELAY, OUTPUT);
    digitalWrite(HEATER_RELAY, HIGH); // OFF
    
    // Explicitly set LED OFF first
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    ledcSetup(ledChannel, freq, resolution);

    feederServo.attach(SERVO_PIN);
    feederServo.write(0);
    feederServo.detach();

    // Init OLED
    Wire.begin(OLED_SDA, OLED_SCL);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Booting...");
    display.display();

    // Init RTC
    Rtc.Begin();

    // Connect WiFi
    
    WiFi.begin(ssid, password);
    int wifiCount = 0;
    while (WiFi.status() != WL_CONNECTED && wifiCount < 20) {
        delay(500);
        Serial.print(".");
        wifiCount++;
    }

    Blynk.config(BLYNK_AUTH_TOKEN);
    
    // Sync Time
    configTime(5 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        RtcDateTime now(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        Rtc.SetDateTime(now);
    }
    
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);
}

bool isTimeInRange(int h, int m, int sh, int sm, int eh, int em) {
    int now = h * 60 + m;
    int start = sh * 60 + sm;
    int end = eh * 60 + em;
    if (start == end) return false;
    if (start < end) return now >= start && now < end;
    return now >= start || now < end; 
}

/************ LOOP ************/
void loop() {
    Blynk.run();
    
    if (!client.connected()) reconnectMqtt();
    client.loop();

    RtcDateTime now = Rtc.GetDateTime();
    int h = now.Hour();
    int m = now.Minute();

    // Automation
    if (!pumpOverride) {
        setPump(isTimeInRange(h, m, pumpSH, pumpSM, pumpEH, pumpEM));
    }
    if (!heaterOverride) {
        setHeater(isTimeInRange(h, m, heaterSH, heaterSM, heaterEH, heaterEM));
    }
    if (!ledOverride) {
        setLED(isTimeInRange(h, m, ledSH, ledSM, ledEH, ledEM));
    }

    // Feeding
    if (h == feedH && m == feedM && !feedDoneToday) {
        feedFish();
        feedDoneToday = true;
    }
    if (h == 0 && m == 0) feedDoneToday = false;

    // Display
    static unsigned long lastOled = 0;
    if (millis() - lastOled > 1000) {
        lastOled = millis();
        display.clearDisplay();
        display.setCursor(0,0);
        display.printf("Time: %02d:%02d\n", h, m);
        display.printf("Pump: %s %s\n", pumpState?"ON":"OFF", pumpOverride?"(M)":"");
        display.printf("Heat: %s %s\n", heaterState?"ON":"OFF", heaterOverride?"(M)":"");
        display.printf("Light: %s %s\n", ledState?"ON":"OFF", ledOverride?"(M)":"");
        display.printf("Blynk: %s\n", Blynk.connected()?"OK":"DQ");
        display.display();
    }
}
