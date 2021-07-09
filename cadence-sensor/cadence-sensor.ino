// Standard Libraries
#include <cstdint>

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

// Local
#include "DisplayManager.h"
#include "DebugSerial.h"
#include "CadenceSensorApp.h"

// Definitions
static void scanCompleteCB(BLEScanResults);
static void notifyCallback(BLERemoteCharacteristic*, uint8_t*, size_t, bool);

// Constants
// Display - Buttons
static constexpr uint8_t BUTTON_A{ 15 };
static constexpr uint8_t BUTTON_B{ 32 };
static constexpr uint8_t BUTTON_C{ 14 };

// Program version
#define VERSION "0.0.1"
static CadenceSensorApp app(scanCompleteCB, notifyCallback);

static void scanCompleteCB(BLEScanResults results) {
  app.setScanComplete();
}

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  app.notify(pBLERemoteCharacteristic, pData, length, isNotify);
}

void setup() {
  DebugSerialStart(115200);
  DebugSerialReady();
  DebugSerialInfo("Starting cadence-sensor version " VERSION " ...");

  if (false == app.initialize())
  {
    DebugSerialErr("App init failed");
    while(1){};
  }

  DebugSerialInfo("Setup completed");
}

void loop() {
  app.step();
  delay(500);
}
