#include "CadenceSensorApp.h"

// Local
#include "DebugSerial.h"

// Types
typedef struct {
  uint32_t prevCumlativeCranks;
  uint16_t prevLastWheelEventTime;
  uint8_t calculatedCadence;
  uint8_t staleness;
} CadenceData;

// Constants
// Bluetooth - speed & cadence UUID
static BLEUUID const CycleSpeedAndCadenceServiceUUID(static_cast<uint16_t>(0x1816));
// Bluetooth - notify UUID
static BLEUUID const NotifyCharacteristicUUID(static_cast<uint16_t>(0x2a5b));
// Sensor - staleness cycles
static constexpr uint8_t SENSOR_STALENESS_LIMIT{ 4 };
static constexpr float_t SENSOR_TIME_RESOLUTION_SCALE{ 1024.0f };
static constexpr float_t SENSOR_TIME_TO_MIN_SCALE{ SENSOR_TIME_RESOLUTION_SCALE / 60.0f };

// Globals
static CadenceData cadenceData{0};

static void scanCompleteCB(BLEScanResults results) {
}

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                           uint8_t* pData,
                           size_t length,
                           bool isNotify) {
  DebugSerialPrintLn("Notify Callback");
  DebugSerialPrint("Length: ");
  DebugSerialPrintLn(length);

  /*bool const hasWheel = static_cast<bool>(pData[0] & 0x1);

  int crankRevIndex = 1;
  int crankTimeIndex = 3;
  if (true == hasWheel) {
    crankRevIndex = 7;
    crankTimeIndex = 9;
  }

  int const cumulativeCrankRev = static_cast<int>((pData[crankRevIndex + 1] << 8) + pData[crankRevIndex]);
  int const lastCrankTime = static_cast<int>((pData[crankTimeIndex + 1] << 8) + pData[crankTimeIndex]);

  int deltaRotations = cumulativeCrankRev - cadenceData.prevCumlativeCranks;
  if (deltaRotations < 0) {
    deltaRotations += 65535;
  }

  int timeDelta = lastCrankTime - cadenceData.prevLastWheelEventTime;
  if (timeDelta < 0) {
    timeDelta += 65535;
  }

  // In Case Cad Drops, we use previous
  // to substitute
  if (timeDelta != 0) {
    cadenceData.staleness = 0;
    float const timeMins = static_cast<float>(timeDelta) / SENSOR_TIME_TO_MIN_SCALE;
    cadenceData.calculatedCadence = static_cast<uint8_t>(static_cast<float>(deltaRotations) / static_cast<float>(timeMins));
  }

  else if ((timeDelta == 0) && (cadenceData.staleness < SENSOR_STALENESS_LIMIT)) {
    cadenceData.staleness += 1;
  } else if (cadenceData.staleness >= SENSOR_STALENESS_LIMIT) {
    cadenceData.calculatedCadence = 255;
  }

  cadenceData.prevCumlativeCranks = cumulativeCrankRev;
  cadenceData.prevLastWheelEventTime = lastCrankTime;*/
}

CadenceSensorApp::CadenceSensorApp()
  : state{ AppState_t::SCAN_DEVICES },
    pBLEScan{ nullptr },
    cadenceSensor{ nullptr },
    display() {}

CadenceSensorApp::~CadenceSensorApp(void) {
  if (nullptr != cadenceSensor) {
    delete cadenceSensor;
    cadenceSensor = nullptr;
  }
}

bool CadenceSensorApp::initialize(void) {
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(this);
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  DebugSerialInfo("CadenceSensorApp init completed");
  return true;
}

void CadenceSensorApp::step(void) {

  DebugSerialVerbose("Starting")

  AppState_t nextState{ AppState_t::NO_STATE };
  switch (state) {
    case AppState_t::SCAN_DEVICES:
      if (true == pBLEScan->start(11, scanCompleteCB, false)) {
        nextState = AppState_t::SCAN_RUNNING;
        DebugSerialInfo("BLE scan started");
      } else {
        // TODO Handle error on start of scanning
        DebugSerialErr("Starting BLE scan");
      }
      break;
    case AppState_t::SCAN_RUNNING:
      // Nothing to do
      DebugSerialPrint(".");
      break;
    case AppState_t::CONNECT_TO_SENSOR:
      DebugSerialPrintLn("");
      if (false == connect()) {
        // TODO - handle error
        DebugSerialErr("Connecting to BLE sensor, retrying scan");
        nextState = AppState_t::SCAN_DEVICES;
      }
      break;
    case AppState_t::SENSOR_CONNECTED:
      DebugSerialInfo("BLE sensor reported connected");
      nextState = AppState_t::DISPLAY_CADENCE;
      break;
    case AppState_t::DISPLAY_CADENCE:
      // TODO - output
      break;
    case AppState_t::SENSOR_DISCONNECT:
      DebugSerialErr("BLE sensor disconnected, retrying connection");
      nextState = AppState_t::CONNECT_TO_SENSOR;
      break;
    default:
      break;
  }

  if (nextState != AppState_t::NO_STATE) {
    state = nextState;
  }
}

bool CadenceSensorApp::connect(void) {
  BLEClient* pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(this);

  // Connect to the remote BLE Server.
  pClient->connect(cadenceSensor);

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(CycleSpeedAndCadenceServiceUUID);
  if (nullptr == pRemoteService) {
    DebugSerialErr("Unable to obtain CSC service");
    pClient->disconnect();
    delete pClient;
    pClient = nullptr;
    return false;
  }

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(NotifyCharacteristicUUID);
  if (nullptr == pRemoteCharacteristic) {
    DebugSerialErr("Unable to obtain Notify characteristic");
    pClient->disconnect();
    delete pClient;
    pClient = nullptr;
    return false;
  }

  if (true == pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  } else {
    DebugSerialErr("Unable to subscribe to notify");
    pClient->disconnect();
    delete pClient;
    pClient = nullptr;
    return false;
  }

  DebugSerialInfo("Successful connection to sensor");
  return true;
}

void CadenceSensorApp::onConnect(BLEClient* pclient) {
  state = AppState_t::SENSOR_CONNECTED;
}

void CadenceSensorApp::onDisconnect(BLEClient* pclient) {
  state = AppState_t::SENSOR_DISCONNECT;
}

void CadenceSensorApp::onResult(BLEAdvertisedDevice advertisedDevice) {
  // We have found a device, let us now see if it contains the service we are looking for.
  if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(CycleSpeedAndCadenceServiceUUID)) {
    DebugSerialInfo("BLE Device with CSC service found");
    pBLEScan->stop();
    pBLEScan->clearResults();
    cadenceSensor = new BLEAdvertisedDevice(advertisedDevice);
    state = AppState_t::CONNECT_TO_SENSOR;
  } else {
    // Not advertising CSC service, ensure we scan again if this is the last device
    DebugSerialInfo("Rejected device");
    if (advertisedDevice.haveName()) {
      DebugSerialPrint("Name: ");
      DebugSerialPrintLn(advertisedDevice.getName().c_str());
    }
    state = AppState_t::SCAN_DEVICES;
  }
}
