
#include "BluetoothManager.h"
#include "LoggingConfig.h"
#include "Tasks.h"
#include "Blackboard.h"

BluetoothManager::BluetoothManager(BLEScanCompleteCB_t pScanCompleteCallBack, BLENotifyCB_t pNotifyCallBack) :
    CycleSpeedAndCadenceServiceUUID(static_cast<uint16_t>(0x1816)),
    NotifyCharacteristicUUID(static_cast<uint16_t>(0x2a5b)),
    pBLEScan{ nullptr },
    cadenceSensor{ nullptr },
    pScanCompletedCB{ pScanCompleteCallBack },
    pNotifyCompletedCB{ pNotifyCallBack },
    scanCount{ 0 },
    _state{ AppState_t::SCAN_DEVICES } {}

BluetoothManager::~BluetoothManager(void) {}

bool BluetoothManager::initialize(void) {
    // Reset all state and counters
  _state = AppState_t::SCAN_DEVICES;
  scanCount = 0;

  if (nullptr != cadenceSensor) {
    delete cadenceSensor;
    cadenceSensor = nullptr;
  }

  Log.noticeln("init starting");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(this);
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  Log.noticeln("BLE init complete");

  Log.noticeln("display init complete");

  Log.noticeln("init completed");

  return true;

}

void BluetoothManager::step(void) {

  AppState_t nextState{ _state };
  switch (_state) {
    case AppState_t::SCAN_DEVICES:
      if (scanCount > (MAX_SCANS - 1 )) {
        nextState = AppState_t::ABORT;
      } else {
        if (true == pBLEScan->start(SCAN_TIME_SECS, pScanCompletedCB, false)) {
          nextState = AppState_t::SCAN_RUNNING;
          scanCount++;
          Log.noticeln("BLE scan started");
        } else {
          // TODO Handle error on start of scanning
          Log.errorln("BLE scan start");
        }
      }
      break;
    case AppState_t::CONNECT_TO_SENSOR:
      if (false == _connect()) {
        // Handle error
        Log.errorln("Connecting to BLE sensor, retrying scan");
        nextState = AppState_t::SCAN_DEVICES;
      } else {
        nextState = AppState_t::NOTIFY_CADENCE;
      }
      break;
    case AppState_t::SENSOR_DISCONNECT:
      Log.errorln("BLE sensor disconnected, retrying scan");
      nextState = AppState_t::SCAN_DEVICES;
      break;
    case AppState_t::SCAN_RUNNING:
    case AppState_t::NOTIFY_CADENCE:
    case AppState_t::ABORT:
      // Nothing to do
      break;
  }

  _state = nextState;
}

bool BluetoothManager::_connect(void) {
  BLEClient* pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(this);

  // Connect to the remote BLE Server.
  pClient->connect(cadenceSensor);

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(CycleSpeedAndCadenceServiceUUID);
  if (nullptr == pRemoteService) {
    Log.errorln("Unable to obtain CSC service");
    pClient->disconnect();
    delete pClient;
    pClient = nullptr;
    return false;
  }

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(NotifyCharacteristicUUID);
  if (nullptr == pRemoteCharacteristic) {
    Log.errorln("Unable to obtain Notify characteristic");
    pClient->disconnect();
    delete pClient;
    pClient = nullptr;
    return false;
  }

  if (true == pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(pNotifyCompletedCB);
  } else {
    Log.errorln("Unable to subscribe to notify");
    pClient->disconnect();
    delete pClient;
    pClient = nullptr;
    return false;
  }

  Log.noticeln("Successful connection to sensor");

  return true;
}

void BluetoothManager::onConnect(BLEClient* pclient) {
}

void BluetoothManager::onDisconnect(BLEClient* pclient) {
  _state = AppState_t::SENSOR_DISCONNECT;
}

void BluetoothManager::onResult(BLEAdvertisedDevice advertisedDevice) {
  // We have found a device, let us now see if it contains the service we are looking for.
  if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(CycleSpeedAndCadenceServiceUUID)) {
    Log.noticeln("BLE Device with CSC service found");
    pBLEScan->stop();
    pBLEScan->clearResults();
    cadenceSensor = new BLEAdvertisedDevice(advertisedDevice);
    _state = AppState_t::CONNECT_TO_SENSOR;
  }
}

void BluetoothManager::setScanComplete(void) {
  if (_state != AppState_t::CONNECT_TO_SENSOR) {
    _state = AppState_t::SCAN_DEVICES;
  }
}

void BluetoothManager::notify(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  uint8_t const flags = pData[0];

  bool const hasWheel = static_cast<bool>(flags & 0x1);
  bool const hasCrank = static_cast<bool>(flags & 0x2);

  if (true == hasCrank) {

    int crankRevIndex = 1;
    int crankTimeIndex = 3;
    if (true == hasWheel) {
      crankRevIndex = 7;
      crankTimeIndex = 9;
    }

    uint16_t cumulativeCrankRev{ 0 };
    uint16_t lastCrankTime{ 0 };

    memcpy(&cumulativeCrankRev, &pData[crankRevIndex], sizeof(uint16_t));
    memcpy(&lastCrankTime, &pData[crankTimeIndex], sizeof(uint16_t));

    Log.noticeln("Cranks: %u", cumulativeCrankRev);
    Log.noticeln("Time: %u", lastCrankTime);

    blackboard.ble.valid = true;
    blackboard.ble.cumlativeCranks = cumulativeCrankRev;
    blackboard.ble.lastWheelEventTime = lastCrankTime;
    blackboard.ble.lastNotifyTime = millis();

  } else {
    Log.errorln("No crank data");
  }
}

void scanCompleteCB(BLEScanResults results) {
  reinterpret_cast<BluetoothManager*>(tasks[BLE_TASK_IDX])->setScanComplete();
}

void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  reinterpret_cast<BluetoothManager*>(tasks[BLE_TASK_IDX])->notify(pBLERemoteCharacteristic, pData, length, isNotify);
}
