// Standard Libraries
#include <cstdint>

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

// Local
#include "DisplayManager.h"
#include "DebugSerial.h"
#include "CadenceSensorApp.h"

typedef struct {
  uint32_t prevCumlativeCranks;
  uint16_t prevLastWheelEventTime;
  uint8_t calculatedCadence;
  uint8_t staleness;
} CadenceData;

// Constants
// Display - Buttons
static constexpr uint8_t BUTTON_A{ 15 };
static constexpr uint8_t BUTTON_B{ 32 };
static constexpr uint8_t BUTTON_C{ 14 };
// Bluetooth - speed & cadence UUID
static BLEUUID const CycleSpeedAndCadenceServiceUUID(static_cast<uint16_t>(0x1816));
// Bluetooth - notify UUID
static BLEUUID const NotifyCharacteristicUUID(static_cast<uint16_t>(0x2a5b));
// Sensor - staleness cycles
static constexpr uint8_t SENSOR_STALENESS_LIMIT{ 4 };
static constexpr float_t SENSOR_TIME_RESOLUTION_SCALE{ 1024.0f };
static constexpr float_t SENSOR_TIME_TO_MIN_SCALE{ SENSOR_TIME_RESOLUTION_SCALE / 60.0f };

// Globals
static boolean doConnect = false;
static boolean connected = false;
// Program version
#define VERSION "0.0.1"
static DisplayManager display;
static BLERemoteCharacteristic* pRemoteCharacteristic{ nullptr };
static BLEAdvertisedDevice* cadenceSensor{ nullptr };
static CadenceData cadenceData{ 0 };

static CadenceSensorApp app;

// Called on connect or disconnect
class ClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    DebugSerialVerbose("onConnect");
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    DebugSerialVerbose("onDisconnect");
  }
};

// Called on advertised server found
class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    DebugSerialVerbose("DebugSerialVerbose");
    DebugSerialInfo("BLE Advertised Device found");

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(CycleSpeedAndCadenceServiceUUID)) {
      DebugSerialInfo("BLE Device with CSC service found");
      BLEDevice::getScan()->stop();
      BLEDevice::getScan()->clearResults();
      cadenceSensor = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
    else {
      DebugSerialInfo("Not advertising CSC service");
    }
  }  // onResult
};

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                           uint8_t* pData,
                           size_t length,
                           bool isNotify) {
  bool const hasWheel = bitRead(pData[0], 0) == 1;

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
  cadenceData.prevLastWheelEventTime = lastCrankTime;
}

bool connectToServer() {
  DebugSerialInfo("Forming a connection to ");
  DebugSerialPrintLn(cadenceSensor->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();
  DebugSerialInfo(" - Created client");

  pClient->setClientCallbacks(new ClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(cadenceSensor);
  DebugSerialInfo(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(CycleSpeedAndCadenceServiceUUID);
  if (pRemoteService == nullptr) {
    DebugSerialErr("Failed to find our service UUID");
    pClient->disconnect();
    delete pClient;
    pClient = nullptr;
    return false;
  }
  DebugSerialPrintLn(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(NotifyCharacteristicUUID);
  if (pRemoteCharacteristic == nullptr) {
    DebugSerialErr("Failed to find our characteristic UUID");
    pClient->disconnect();
    delete pClient;
    pClient = nullptr;
    return false;
  }
  DebugSerialInfo(" - Found our characteristic");

  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  } else {
    DebugSerialErr("Failed to find our notify UUID");
    pClient->disconnect();
    delete pClient;
    pClient = nullptr;
    return false;
  }

  return true;
}

void setup() {
  DebugSerialStart(115200);
  DebugSerialReady();
  DebugSerialInfo("Starting cadence-sensor version " VERSION " ...");

  if (false == app.initialize())
  {
    while(1){};
  }

  display.splash();
  display.clear();
  display.insert_line("Starting cadence-sensor version " VERSION " ...");
  display.println_lines();
  display.display();
  delay(1000);

  cadenceData.prevCumlativeCranks = 0;
  cadenceData.prevLastWheelEventTime = 0;
  cadenceData.calculatedCadence = 0;
  cadenceData.staleness = 0;

  // Enable BLE
  DebugSerialInfo("Starting BLE scanning");
  BLEDevice::init("");
  // Scan for cadence sensor
  BLEDevice::getScan()->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  BLEDevice::getScan()->setInterval(1349);
  BLEDevice::getScan()->setWindow(449);
  BLEDevice::getScan()->setActiveScan(true);
  BLEDevice::getScan()->start(11, false);

  DebugSerialInfo("Setup completed");
}

void loop() {
  app.step();
  if (false == connected) {
    DebugSerialInfo("Not connected, rescanning");
    BLEDevice::getScan()->stop();
    BLEDevice::getScan()->clearResults();
    BLEDevice::getScan()->start(5, false);
  }
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (true == doConnect) {
    connected = connectToServer();
    if (true == connected) {
      DebugSerialInfo("Connected to sensor");
      doConnect = false;
    }
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (true == connected) {
    // Change display to cadence mode
    // Set displayed cadence
    DebugSerialPrintLn(cadenceData.calculatedCadence);
    // Display calculated cadence
  }
  delay(1000);
}
