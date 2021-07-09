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
typedef void (BLENotifyCB_t)(BLERemoteCharacteristic*, uint8_t*, size_t, bool);

class CadenceSensorApp : public IApplication, public BLEAdvertisedDeviceCallbacks, public BLEClientCallbacks {
  public:
    CadenceSensorApp(BLEScanCompleteCB_t, BLENotifyCB_t);
    ~CadenceSensorApp(void);
    bool initialize(void);
    void step(void);

    void onConnect(BLEClient*);
    void onDisconnect(BLEClient*);
    void onResult(BLEAdvertisedDevice);
    void setScanComplete(void);
    void notify(BLERemoteCharacteristic*, uint8_t*, size_t, bool);

  private:
    enum class AppState_t : uint8_t {
      SCAN_DEVICES = 0,
      SCAN_RUNNING,
      CONNECT_TO_SENSOR,
      SENSOR_CONNECTED,
      DISPLAY_CADENCE,
      SENSOR_DISCONNECT,
      DISPLAY_BATTERY,
      ABORT,
    };
    AppState_t state;

    BLEScan* pBLEScan;
    BLEAdvertisedDevice* cadenceSensor;
    BLEScanCompleteCB_t* pScanCompletedCB;
    BLENotifyCB_t* pNotifyCompletedCB;

    DisplayManager display;

    bool connect(void);

    uint8_t scanCount;
    bool aborted;
};

#endif
