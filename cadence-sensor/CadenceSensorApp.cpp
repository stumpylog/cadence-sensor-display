#include "CadenceSensorApp.h"



CadenceSensorApp::CadenceSensorApp(void) :
    state{AppState_t::SCAN_DEVICES} {}

CadenceSensorApp::~CadenceSensorApp(void) {}

bool CadenceSensorApp::initialize(void) {
  return true;
}

void CadenceSensorApp::step(void) {
  AppState_t nextState{AppState_t::NO_STATE};
  switch (state) {
    case AppState_t::SCAN_DEVICES:
      break;
    default:
      break;
  }

  if (nextState != AppState_t::NO_STATE) {
    state = nextState;
  }

}
