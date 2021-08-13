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

// Types
typedef struct {
  uint32_t prevCumlativeCranks;
  uint16_t prevLastWheelEventTime;
  uint8_t calculatedCadence;
  uint8_t lastDisplayedCadence;
  uint8_t staleness;
} CadenceData;
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

    bool sleep;

  private:
    // Types
    enum class AppState_t : uint8_t {
      SCAN_DEVICES = 0,
      SCAN_RUNNING,
      CONNECT_TO_SENSOR,
      DISPLAY_CADENCE,
      SENSOR_DISCONNECT,
      DISPLAY_BATTERY,
      ABORT_NOTIFY,
      ABORT,
    };

    // Constants
    // Bluetooth - speed & cadence UUID
    BLEUUID const CycleSpeedAndCadenceServiceUUID;
    // Bluetooth - notify UUID
    BLEUUID const NotifyCharacteristicUUID;
    // Sensor - staleness cycles
    static constexpr uint8_t SENSOR_STALENESS_LIMIT{ 4 };
    // Sensor - last event time resolution
    static constexpr float_t SENSOR_TIME_RESOLUTION{ 1024.0f };
    static constexpr float_t SECONDS_PER_MINUTE{ 60.0f };

    // Methods
    bool connect(void);

    // Fields
    // Current state
    AppState_t state;
    // BLE related
    BLEScan* pBLEScan;
    BLEAdvertisedDevice* cadenceSensor;
    BLEScanCompleteCB_t* pScanCompletedCB;
    BLENotifyCB_t* pNotifyCompletedCB;
    // Display
    DisplayManager display;

    // BLE scanning counters
    uint8_t scanCount;
    uint8_t scanCycles;

    // Sensor data
    uint16_t prevCumlativeCranks;
    uint16_t prevLastWheelEventTime;
    uint16_t calculatedCadence;
    uint16_t lastDisplayedCadence;
    uint8_t sensorStaleness;
};

#endif
