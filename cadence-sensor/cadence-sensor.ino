// Standard Libraries
#include <cstdint>

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

// Local
#include "DisplayManager.h"
#include "LoggingConfig.h"
#include "CadenceSensorApp.h"

// Declaration
static void scanCompleteCB(BLEScanResults);
static void notifyCallback(BLERemoteCharacteristic*, uint8_t*, size_t, bool);

// Constants
// Display - Buttons
static constexpr uint8_t BUTTON_A{ GPIO_NUM_15 };
static constexpr uint8_t BUTTON_B{ GPIO_NUM_32 };
static constexpr uint8_t BUTTON_C{ GPIO_NUM_14 };

// Program version
#define VERSION "0.0.2"
static CadenceSensorApp app(scanCompleteCB, notifyCallback);

DisplayManager display;

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

  if (false == app.initialize())
  {
    Log.fatalln("App init failed");
    while (1) {};
  }

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(BUTTON_A), 0);

  Log.noticeln("Setup completed");
}

void loop() {
  app.step();
  if ((LOW == digitalRead(BUTTON_C)) || true == app.sleep)
  {
    Log.noticeln("Starting deep sleep");
    esp_deep_sleep_start();
  }
  delay(100);
  yield();
}
