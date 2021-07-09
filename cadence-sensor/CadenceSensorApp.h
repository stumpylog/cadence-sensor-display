#ifndef CADENCE_SENSOR_APP_H
#define CADENCE_SENSOR_APP_H

// Standard Libraries
#include <cstdint>
#include <cmath>

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

// Local
#include "IApplication.h"
#include "DisplayManager.h"

typedef void (BLEScanCompleteCB_t)(BLEScanResults);

class CadenceSensorApp : public IApplication, public BLEAdvertisedDeviceCallbacks, public BLEClientCallbacks {
  public:
    CadenceSensorApp();
    ~CadenceSensorApp(void);
    bool initialize(void);
    void step(void);

    void onConnect(BLEClient* pclient);
    void onDisconnect(BLEClient* pclient);
    void onResult(BLEAdvertisedDevice advertisedDevice);

  private:
    enum class AppState_t : uint8_t {
      NO_STATE = 0,
      SCAN_DEVICES,
      SCAN_RUNNING,
      CONNECT_TO_SENSOR,
      SENSOR_CONNECTED,
      DISPLAY_CADENCE,
      SENSOR_DISCONNECT,
      DISPLAY_BATTERY,
    };
    AppState_t state;

    BLEScan* pBLEScan;
    BLEAdvertisedDevice* cadenceSensor;
    DisplayManager display;

    bool connect(void);
};

#endif
