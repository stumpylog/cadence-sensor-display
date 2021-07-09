#include "CadenceSensorApp.h"

CadenceSensorApp::CadenceSensorApp(BLEScanCompleteCB_t pScanCompleteCallBack) :
    state{AppState_t::SCAN_DEVICES},
    pBLEScan{nullptr},
    pScanCompletedCB{pScanCompleteCallBack} {}

CadenceSensorApp::~CadenceSensorApp(void) {}

bool CadenceSensorApp::initialize(void) {
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  return true;
}

void CadenceSensorApp::step(void) {
  AppState_t nextState{AppState_t::NO_STATE};
  switch (state) {
    case AppState_t::SCAN_DEVICES:
      if (true == pBLEScan->start(11, pScanCompletedCB, false)) {
        // TODO Handle error on start of scanning
      }
      else {
        nextState = AppState_t::SCAN_RUNNING;
      }
      break;
    case AppState_t::SCAN_RUNNING:
      // Nothing to do
      break;
    case AppState_t::CONNECT_TO_SENSOR:
      break;
    case AppState_t::DISPLAY_CADENCE:
      break;
    default:
      break;
  }

  if (nextState != AppState_t::NO_STATE) {
    state = nextState;
  }

}
