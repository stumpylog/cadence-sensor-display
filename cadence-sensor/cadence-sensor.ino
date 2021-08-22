// Standard Libraries
#include <cstdint>

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

// Local
#include "LoggingConfig.h"
#include "IApplication.h"
#include "Version.h"
#include "Tasks.h"


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
        yield();
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
