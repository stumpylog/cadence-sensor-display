#include "CadenceSensorApp.h"

// Local
#include "LoggingConfig.h"

// Types

// Globals

CadenceSensorApp::CadenceSensorApp(BLEScanCompleteCB_t pScanCompleteCallBack, BLENotifyCB_t pNotifyCallBack)
  : sleep { false },
    CycleSpeedAndCadenceServiceUUID(static_cast<uint16_t>(0x1816)),
    NotifyCharacteristicUUID(static_cast<uint16_t>(0x2a5b)),
    state{ AppState_t::SCAN_DEVICES },
    pBLEScan{ nullptr },
    cadenceSensor{ nullptr },
    pScanCompletedCB{ pScanCompleteCallBack },
    pNotifyCompletedCB{ pNotifyCallBack },
    display(),
    scanCount{ 0 },
    prevCumlativeCranks{ 0 },
    prevLastWheelEventTime{ 0 },
    calculatedCadence{ 0 },
    lastDisplayedCadence{ 0 },
    sensorStaleness{ 0 } { }

CadenceSensorApp::~CadenceSensorApp(void) {
  if (nullptr != cadenceSensor) {
    delete cadenceSensor;
    cadenceSensor = nullptr;
  }
}

bool CadenceSensorApp::initialize(void) {

  // Reset all state and counters
  sleep = false;
  state = AppState_t::SCAN_DEVICES;
  scanCount = 0;
  prevCumlativeCranks = 0;
  prevLastWheelEventTime = 0;
  calculatedCadence = 0;
  lastDisplayedCadence = 0;
  sensorStaleness = 0;

  if (nullptr != cadenceSensor) {
    delete cadenceSensor;
    cadenceSensor = nullptr;
  }

  Log.noticeln("init starting");
  display.insert_line("init starting");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(this);
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  Log.noticeln("BLE init complete");

  display.initialize();
  Log.noticeln("display init complete");

  Log.noticeln("init completed");
  display.insert_line("init completed");
  display.println_lines();

  return true;
}

void CadenceSensorApp::step(void) {

  Log.verboseln("Starting step");

  AppState_t nextState{ state };
  switch (state) {
    case AppState_t::SCAN_DEVICES:
      if (scanCount > (MAX_SCANS - 1 )) {
        nextState = AppState_t::ABORT_NOTIFY;
      } else {
        if (true == pBLEScan->start(SCAN_TIME_SECS, pScanCompletedCB, false)) {
          nextState = AppState_t::SCAN_RUNNING;
          scanCount++;
          Log.noticeln("BLE scan started");
          display.insert_line("BLE scan started");
          display.println_lines();
        } else {
          // TODO Handle error on start of scanning
          Log.errorln("BLE scan start");
        }
      }
      break;
    case AppState_t::SCAN_RUNNING:
      // Nothing to do
      break;
    case AppState_t::CONNECT_TO_SENSOR:
      if (false == connect()) {
        // Handle error
        Log.errorln("Connecting to BLE sensor, retrying scan");
        nextState = AppState_t::SCAN_DEVICES;
      } else {
        nextState = AppState_t::DISPLAY_CADENCE;
      }
      break;
    case AppState_t::DISPLAY_CADENCE:
      if (calculatedCadence != lastDisplayedCadence) {
        Log.noticeln("Cadence: %u", calculatedCadence);
        display.display_cadence(calculatedCadence);
        lastDisplayedCadence =  calculatedCadence;
      }
      break;
    case AppState_t::SENSOR_DISCONNECT:
      Log.errorln("BLE sensor disconnected, retrying scan");
      nextState = AppState_t::SCAN_DEVICES;
      break;
    case AppState_t::ABORT_NOTIFY:
      Log.errorln("Unable to locate sensor in 10 scans, aborting");
      display.insert_line("BLE scan aborted");
      display.println_lines();
      nextState = AppState_t::ABORT;
      break;
    case AppState_t::ABORT:
      display.insert_line("Entering deep sleep");
      display.println_lines();
      sleep = true;
      break;
    default:
      break;
  }

  state = nextState;
}

bool CadenceSensorApp::connect(void) {
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
  display.insert_line("connected to sensor");
  display.println_lines();

  return true;
}

void CadenceSensorApp::onConnect(BLEClient* pclient) {
}

void CadenceSensorApp::onDisconnect(BLEClient* pclient) {
  state = AppState_t::SENSOR_DISCONNECT;
}

void CadenceSensorApp::onResult(BLEAdvertisedDevice advertisedDevice) {
  // We have found a device, let us now see if it contains the service we are looking for.
  if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(CycleSpeedAndCadenceServiceUUID)) {
    Log.noticeln("BLE Device with CSC service found");
    display.insert_line("CSC service found");
    display.println_lines();
    pBLEScan->stop();
    pBLEScan->clearResults();
    cadenceSensor = new BLEAdvertisedDevice(advertisedDevice);
    state = AppState_t::CONNECT_TO_SENSOR;
  }
}

void CadenceSensorApp::setScanComplete(void) {
  if (state != AppState_t::CONNECT_TO_SENSOR) {
    state = AppState_t::SCAN_DEVICES;
  }
}

void CadenceSensorApp::notify(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  Log.verboseln("Notify Callback");

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
    Log.noticeln("P-Cranks: %u", prevCumlativeCranks);
    Log.noticeln("Time: %u", lastCrankTime);
    Log.noticeln("P-Time: %u", prevLastWheelEventTime);

    int32_t deltaRotations = cumulativeCrankRev - prevCumlativeCranks;
    if (deltaRotations < 0) {
      // Roll over
      Log.noticeln("Rotations rollover");
      deltaRotations += 0xFFFF;
    }

    int32_t timeDelta = lastCrankTime - prevLastWheelEventTime;
    if (timeDelta < 0) {
      // Roll over
      Log.noticeln("Time rollover");
      timeDelta += 0xFFFF;
    }

    if (timeDelta != 0) {
      sensorStaleness = 0;
      // Convert event time delta to a time in minutes
      float const timeMins = static_cast<float>(timeDelta) / SENSOR_TIME_RESOLUTION / SECONDS_PER_MINUTE;
      // Calculate new RPM
      calculatedCadence = static_cast<uint8_t>(static_cast<float>(deltaRotations) / timeMins);
      // Save current data as previous for the next time
      prevCumlativeCranks = cumulativeCrankRev;
      prevLastWheelEventTime = lastCrankTime;
    } else if ((0 == timeDelta) && (sensorStaleness <= SENSOR_STALENESS_LIMIT)) {
      // Keep the same value as before
      sensorStaleness++;
    } else if (sensorStaleness > SENSOR_STALENESS_LIMIT) {
      // Very stale, set to 0
      calculatedCadence = 0;
    }

  } else {
    Log.errorln("No crank data");
  }
}
