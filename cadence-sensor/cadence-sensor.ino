// Standard Libraries
#include <cstdint>

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

// Local
#include "DisplayManager.h"
#include "DebugSerial.h"
#include "CadenceSensorApp.h"

// Constants
// Display - Buttons
static constexpr uint8_t BUTTON_A{ 15 };
static constexpr uint8_t BUTTON_B{ 32 };
static constexpr uint8_t BUTTON_C{ 14 };

// Program version
#define VERSION "0.0.1"
static DisplayManager display;
static CadenceSensorApp app;

void setup() {
  DebugSerialStart(115200);
  DebugSerialReady();
  DebugSerialInfo("Starting cadence-sensor version " VERSION " ...");

  if (false == app.initialize())
  {
    DebugSerialErr("App init failed");
    while(1){};
  }

  display.splash();
  display.clear();
  display.insert_line("Starting cadence-sensor version " VERSION " ...");
  display.println_lines();
  display.display();
  delay(1000);

  DebugSerialInfo("Setup completed");
}

void loop() {
  app.step();
  delay(500);
}
