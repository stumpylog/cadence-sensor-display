#include "DisplayManager.h"

// Arduino Libraries
#include <Wire.h>

// Standard Libraries
#include <cstring>

// Local
#include "LoggingConfig.h"
#include "Blackboard.h"
#include "Version.h"

DisplayManager::DisplayManager(void)
  : _display(DISPLAY_HEIGHT, DISPLAY_WIDTH, &Wire),
    _state{ AppState_t::DISP_VERSION_STATE },
    _state_ticks{ 0 } {}

DisplayManager::~DisplayManager(void) {}

bool DisplayManager::initialize(void) {
  bool passed{ true };

  _state = AppState_t::DISP_VERSION_STATE;

  if (false == _display.begin(DISPLAY_ADDR, true)) {
    Log.errorln("display.begin");
    passed = false;
  } else {
    _display.setRotation(1);
    _display.setTextSize(1);
    _display.setTextColor(SH110X_WHITE);
    _display.setCursor(0, 0);
    _state_ticks = 0;
    _clear();
  }

  return passed;
}

void DisplayManager::step(void) {
  AppState_t next_state{ _state };

  switch (_state) {
    case AppState_t::DISP_VERSION_STATE:
      _display.println("Version: " VERSION "");
      _display.display();
      next_state = AppState_t::VERSION_TRANSITION;
      _state_ticks = 0;
      break;
    case AppState_t::VERSION_TRANSITION:
      if (_state_ticks > VERSION_DISPLAY_TICKS) {
        next_state = AppState_t::DISP_VOLTAGE_STATE;
        _state_ticks = 0;
      }
      break;
    case AppState_t::DISP_VOLTAGE_STATE:
      if (true == blackboard.power.valid) {
        _display.print("Battery: ");
        _display.println(blackboard.power.percent);
        _display.display();
      } else {
        _display.println("No battery info");
        _display.display();
      }
      next_state = AppState_t::VOLTAGE_TRANSITION;
      break;
    case AppState_t::VOLTAGE_TRANSITION:
      if (_state_ticks > POWER_DISPLAY_TICKS) {
        next_state = AppState_t::CADENCE_SETUP;
        _state_ticks = 0;
      }
      break;
    case AppState_t::CADENCE_SETUP:
      if (true == blackboard.cadence.valid) {
        _clear();
        _display.setTextSize(CADENCE_FONT_SIZE);
        next_state = AppState_t::DISP_CADENCE_STATE;
      } else {
        next_state = AppState_t::DISP_NO_CADENCE;
      }
      break;
    case AppState_t::DISP_NO_CADENCE:
      if (false == blackboard.cadence.valid) {
        _display.println("No cadence, waiting");
        _display.display();
        next_state = AppState_t::WAIT_CADENCE;
      }
      break;
    case AppState_t::WAIT_CADENCE:
      if (true == blackboard.cadence.valid) {
        next_state = AppState_t::CADENCE_SETUP;
      } else if (true == blackboard.ble.aborted) {
        next_state = AppState_t::DISP_BLE_ABORT;
      } else if (true == blackboard.power.sleep) {
        next_state = AppState_t::DISP_SLEEPING;
      }
      break;
    case AppState_t::DISP_CADENCE_STATE:
      if (true == blackboard.cadence.valid) {
        // Clear old data
        _display.clearDisplay();

        // Print cadence
        _display.setTextSize(4);
        _display.setCursor(CADENCE_FONT_CENTER_X, CADENCE_FONT_CENTER_Y);
        _display.print(blackboard.cadence.cadence);

        // Print line seps
        _display.writeFastVLine(73, 0, 64, 1);
        _display.writeFastVLine(74, 0, 64, 1);

        // Print distance in miles
        _display.setCursor(80, 28);
        _display.setTextSize(2);
        _display.print(blackboard.cadence.distance, 2);

        // Actually show it all
        _display.display();
      }
      if (true == blackboard.power.sleep) {
        _clear();
        next_state = AppState_t::DISP_SLEEPING;
      }
      break;
    case AppState_t::DISP_BLE_ABORT:
      _display.println("No device found");
      _display.display();
      next_state = AppState_t::BLE_ABORT;
      break;
    case AppState_t::BLE_ABORT:
      if (true == blackboard.power.sleep) {
        next_state = AppState_t::DISP_SLEEPING;
      }
      break;
    case AppState_t::DISP_SLEEPING:
      _display.println("Entering sleep");
      Log.noticeln("Entering sleep");
      _display.display();
      next_state = AppState_t::SLEEP;
      break;
    case AppState_t::SLEEP:
      break;
  }

  _state_ticks++;

  _state = next_state;
}

void DisplayManager::_clear(void) {
  _display.clearDisplay();
  _display.display();
}
