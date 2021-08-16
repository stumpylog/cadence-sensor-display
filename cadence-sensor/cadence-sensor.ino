// Standard Libraries
#include <cstdint>

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

// Local
#include "DisplayManager.h"
#include "LoggingConfig.h"
#include "CadenceSensorApp.h"
#include "IApplication.h"
#include "Version.h"

// Declaration
static void scanCompleteCB(BLEScanResults);
static void notifyCallback(BLERemoteCharacteristic*, uint8_t*, size_t, bool);

// Constants
// Display - Buttons
static constexpr uint8_t BUTTON_A{ GPIO_NUM_15 };
static constexpr uint8_t BUTTON_B{ GPIO_NUM_32 };
static constexpr uint8_t BUTTON_C{ GPIO_NUM_14 };
static constexpr uint8_t TASK_COUNT { 1 };


static CadenceSensorApp app(scanCompleteCB, notifyCallback);

static IApplication* tasks[TASK_COUNT] = {
  new DisplayManager(),
};

static void scanCompleteCB(BLEScanResults results) {
  app.setScanComplete();
}

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  app.notify(pBLERemoteCharacteristic, pData, length, isNotify);
}

void setup() {
#ifndef DISABLE_LOGGING
  Serial.begin(115200);
#endif
  Log.begin(LOG_LEVEL_INFO, &Serial);
  Log.noticeln("Starting cadence-sensor version " VERSION " ...");

  for (int8_t idx = 0; idx < TASK_COUNT; idx++) {
    Log.noticeln("Initializing app %d", idx);

    if (false == tasks[idx]->initialize()) {

      Log.fatalln("app %d init failed", idx);
      while (1) {
        delay(500);
      }
    }
  }

  Log.noticeln("Setup completed");
}

void loop() {

  for (int8_t idx = 0; idx < TASK_COUNT; idx++) {
    tasks[idx]->step();
  }

  delay(100);
  yield();
}
