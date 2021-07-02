// Standard Libraries
#include <stdint.h>

// Arduino Libraries
#include <Wire.h>

// Display Librarues
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Constants
static constexpr uint8_t BUTTON_A{15};
static constexpr uint8_t BUTTON_B{32};
static constexpr uint8_t BUTTON_C{14};
static constexpr uint8_t DISPLAY_ADDR{0x3C};
#define VERSION "0.0.1"

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

void setup() {
  display.begin(DISPLAY_ADDR, true);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();

  display.setRotation(1);

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.print("Starting cadence-sensor version " VERSION " ...");
  display.display();
  delay(2);

  // Enable BLE
  // Scan for cadence sensor
  // Not found, suggest pedaling
  //    Retry scanning
  // Found
  //    Output information
}

void loop() {
  // Handle stale data
  // Display status
  // Display cadence
}
