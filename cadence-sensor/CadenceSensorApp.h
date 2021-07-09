#ifndef CADENCE_SENSOR_APP_H
#define CADENCE_SENSOR_APP_H

// Standard Libraries
#include <cstdint>

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

#include "IApplication.h"

typedef void (BLEScanCompleteCB_t)(BLEScanResults);

class CadenceSensorApp : public IApplication{
  public:
    CadenceSensorApp(BLEScanCompleteCB_t);
    ~CadenceSensorApp(void);
    bool initialize(void);
    void step(void);

    void notifyScanCompleted();

  private:
    enum class AppState_t : uint8_t {
      NO_STATE = 0,
      SCAN_DEVICES,
      SCAN_RUNNING,
      CONNECT_TO_SENSOR,
      DISPLAY_CADENCE,
      DISPLAY_BATTERY,
      LOST_CONNECTION,
    };
    AppState_t state;

    BLEScan* pBLEScan;
    BLEScanCompleteCB_t* pScanCompletedCB;

    void connect(void);
};

#endif
