// Standard Libraries
#include <stdint.h>

// Arduino Libraries
#include <BLEDevice.h>
#include <BLEUUID.h>

// Local
#include "DisplayManager.h"
#include "DebugSerial.h"

// Constants
// Display - Buttons
static constexpr uint8_t BUTTON_A{ 15 };
static constexpr uint8_t BUTTON_B{ 32 };
static constexpr uint8_t BUTTON_C{ 14 };
//Bluetooth - speed & cadence UUID
static BLEUUID const CycleSpeedAndCadenceServiceUUID(static_cast<uint16_t>(0x1816));
// Bluetooth - notify UUID
static BLEUUID const NotifyCharacteristicUUID(static_cast<uint16_t>(0x2a5b));

// Program version
#define VERSION "0.0.1"

// Globals

static boolean doConnect = false;
static boolean connected = false;

static DisplayManager display;
static BLERemoteCharacteristic* pRemoteCharacteristic{ nullptr };
static BLEAdvertisedDevice* myDevice{ nullptr };
static BLEScan* scanner{ nullptr };

// Called on connect or disconnect
class ClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    DebugSerialInfo("onDisconnect");
  }
};

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    DebugSerialInfo("BLE Advertised Device found: ");
    DebugSerialInfo(advertisedDevice.toString().c_str());

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
  DebugSerialPrint("Notify callback for characteristic ");
  DebugSerialPrint(pBLERemoteCharacteristic->getUUID().toString().c_str());
  DebugSerialPrint(" of data length ");
  DebugSerialPrintLn(length);
  DebugSerialPrint("data: ");
  DebugSerialPrintLn((char*)pData);
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
  if ((true == doConnect) && (true == connectToServer()) {
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (true == connected) {
    // Display calculated cadence
  }
  delay(500);
}
