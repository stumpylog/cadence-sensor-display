// Standard Libraries
#include <stdint.h>

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

// Local
#include "DisplayManager.h"
#include "DebugSerial.h"

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
static constexpr uint8_t SENSOR_STALENESS_LIMIT{4};
static constexpr float_t SENSOR_TIME_RESOLUTION_SCALE{1024.0f};
static constexpr float_t SENSOR_TIME_TO_MIN_SCALE{SENSOR_TIME_RESOLUTION_SCALE / 60.0f};

// Globals
static boolean doConnect = false;
static boolean connected = false;
// Program version
#define VERSION "0.0.1"
static DisplayManager display;
static BLERemoteCharacteristic* pRemoteCharacteristic{ nullptr };
static BLEAdvertisedDevice* myDevice{ nullptr };
static BLEScan* scanner{ nullptr };
static CadenceData cadenceData{0};

// Called on connect or disconnect
class ClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    DebugSerialInfo("onDisconnect");
  }
};

// Called on advertised server found
class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    DebugSerialInfo("BLE Advertised Device found: ");
    DebugSerialPrintLn(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(CycleSpeedAndCadenceServiceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
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
  if(true == hasWheel)
  {
    crankRevIndex = 7;
    crankTimeIndex = 9;
  }

  int const cumulativeCrankRev = static_cast<int>((pData[crankRevIndex + 1] << 8) + pData[crankRevIndex]);
  int const lastCrankTime = static_cast<int>((pData[crankTimeIndex + 1] << 8) + pData[crankTimeIndex]);

  int deltaRotations = cumulativeCrankRev - cadenceData.prevCumlativeCranks;
  if (deltaRotations < 0)
  {
    deltaRotations += 65535;
  }

  int timeDelta = lastCrankTime - cadenceData.prevLastWheelEventTime;
  if (timeDelta < 0)
  {
    timeDelta += 65535;
  }

  // In Case Cad Drops, we use PrevRPM
  // to substitute (up to 4 seconds before reporting 0)
  if (timeDelta != 0)
  {
      cadenceData.staleness = 0;
      float const timeMins = static_cast<float>(timeDelta) / SENSOR_TIME_TO_MIN_SCALE;
      cadenceData.calculatedCadence = static_cast<uint8_t>(static_cast<float>(deltaRotations) / timeMins);
  }

  else if ((timeDelta == 0) && (cadenceData.staleness < SENSOR_STALENESS_LIMIT))
  {
      cadenceData.staleness += 1;
  }
  else if (cadenceData.staleness >= SENSOR_STALENESS_LIMIT)
  {
      cadenceData.calculatedCadence = 255;
  }

  cadenceData.prevCumlativeCranks = cumulativeCrankRev;
  cadenceData.prevLastWheelEventTime = lastCrankTime;

}

bool connectToServer() {
  DebugSerialPrint("Forming a connection to ");
  DebugSerialPrintLn(myDevice->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();
  DebugSerialPrintLn(" - Created client");

  pClient->setClientCallbacks(new ClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  DebugSerialPrintLn(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(CycleSpeedAndCadenceServiceUUID);
  if (pRemoteService == nullptr) {
    DebugSerialPrint("Failed to find our service UUID: ");
    pClient->disconnect();
    return false;
  }
  DebugSerialPrintLn(" - Found our service");


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(NotifyCharacteristicUUID);
  if (pRemoteCharacteristic == nullptr) {
    DebugSerialPrint("Failed to find our characteristic UUID: ");
    pClient->disconnect();
    return false;
  }
  DebugSerialPrintLn(" - Found our characteristic");

  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  } else {
    pClient->disconnect();
    return false;
  }

  connected = true;
  return true;
}

void setup() {
  DebugSerialStart(115200);
  DebugSerialReady();
  DebugSerialInfo("Starting cadence-sensor version " VERSION " ...");

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
  BLEDevice::init("");
  // Scan for cadence sensor
  scanner = BLEDevice::getScan();
  scanner->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  scanner->setInterval(1349);
  scanner->setWindow(449);
  scanner->setActiveScan(true);
}

void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if ((true == doConnect) && (true == connectToServer())) {
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (true == connected) {
    // Change display to cadence mode
    // Set displayed cadence
    // Display calculated cadence
  }
  delay(500);
}
