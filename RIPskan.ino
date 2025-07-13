#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define PIR_PIN 4        // PIR sensor pin

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool motionDetected = false;
unsigned long lastMotionTime = 0;
const unsigned long debounceTime = 2000; // 2-sec cooldown

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED ERROR");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Calibrating...");
  display.display();

  Serial.println("Calibrating PIR (wait 60s)...");
  delay(60000); // 60-sec calibration
  Serial.println("READY");
  display.clearDisplay();
  display.println("READY");
  display.display();
}

void loop() {
  bool currentReading = digitalRead(PIR_PIN);
  
  // Debug: Print RAW sensor value (0 or 1)
  Serial.print("PIR RAW: ");
  Serial.println(currentReading);

  // Motion detected (with debounce)
  if (currentReading == HIGH) {
    if (!motionDetected) {
      motionDetected = true;
      lastMotionTime = millis();
      Serial.println(">> MOTION DETECTED");
      updateDisplay("DETECTED");
    }
  } 
  // Motion stopped (after debounce delay)
  else if (motionDetected && (millis() - lastMotionTime > debounceTime)) {
    motionDetected = false;
    Serial.println(">> MOTION ENDED");
    updateDisplay("quiet");
  }

  delay(200); // Small delay to avoid flooding Serial
}

void updateDisplay(String status) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Motion: ");
  display.println(status);
  display.display();
}