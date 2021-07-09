#ifndef CADENCE_SENSOR_APP_H
#define CADENCE_SENSOR_APP_H

// Standard Libraries
#include <cstdint>

// Arduino Libraries
#include <BLEUUID.h>

#include "IApplication.h"

class CadenceSensorApp : public IApplication{
  public:
    CadenceSensorApp(void);
    ~CadenceSensorApp(void);
    bool initialize(void);
    void step(void);

  private:
    enum class AppState_t : uint8_t {
      NO_STATE = 0,
      SCAN_DEVICES,
      CONNECT_TO_DEVICE,
      DISPLAY_CADENCE,
      DISPLAY_BATTERY,
      LOST_CONNECTION,
    };
    AppState_t state;
};

#endif
