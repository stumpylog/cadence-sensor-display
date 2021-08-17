#pragma once

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

// Local
#include "IApplication.h"

typedef void (BLEScanCompleteCB_t)(BLEScanResults);
typedef void (BLENotifyCB_t)(BLERemoteCharacteristic*, uint8_t*, size_t, bool);

class BluetoothManager : public IApplication, public BLEAdvertisedDeviceCallbacks, public BLEClientCallbacks  {
  public:
    BluetoothManager(BLEScanCompleteCB_t, BLENotifyCB_t);
    ~BluetoothManager(void);

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
      NOTIFY_CADENCE,
      SENSOR_DISCONNECT,
      ABORT,
    };

    // Constants
    // Bluetooth - speed & cadence UUID
    BLEUUID const CycleSpeedAndCadenceServiceUUID;
    // Bluetooth - notify UUID
    BLEUUID const NotifyCharacteristicUUID;
    static constexpr uint8_t SCAN_TIME_SECS { 30 };
    static constexpr uint8_t MAX_SCANS { 10 };

    BLEScan* pBLEScan;
    BLEAdvertisedDevice* cadenceSensor;
    BLEScanCompleteCB_t* pScanCompletedCB;
    BLENotifyCB_t* pNotifyCompletedCB;

    // BLE scanning counter
    uint8_t scanCount;

    AppState_t _state;

    bool _connect(void);

};


void scanCompleteCB(BLEScanResults);

void notifyCallback(BLERemoteCharacteristic*, uint8_t*, size_t, bool);
