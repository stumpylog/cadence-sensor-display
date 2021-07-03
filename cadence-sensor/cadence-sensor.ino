// Standard Libraries
#include <stdint.h>

// Arduino Libraries
#include <BLEDevice.h>

// Local
#include "DisplayManager.h"
#include "DebugSerial.h"

// Constants
// Display - Buttons
static constexpr uint8_t BUTTON_A{15};
static constexpr uint8_t BUTTON_B{32};
static constexpr uint8_t BUTTON_C{14};

// Program version
#define VERSION "0.0.1"

// Globals
static DisplayManager display = DisplayManager();

void setup() {
  DebugSerialStart(115200);
  DebugSerialReady();
  DebugSerialInfo("Starting cadence-sensor version " VERSION " ...");
  display.splash();
  display.clear();
  display.insert_line("Starting cadence-sensor version " VERSION " ...");
  display.println_lines();
  display.display();
  delay(1000);

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