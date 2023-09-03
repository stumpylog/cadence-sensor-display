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
  : _display(128, 128, &Wire, -1),
    _state{ AppState_t::DISP_VERSION_STATE },
    _state_ticks{ 0 } {}

DisplayManager::~DisplayManager(void) {}

bool DisplayManager::initialize(void) {
  bool passed{ true };

  _state = AppState_t::DISP_VERSION_STATE;

  pinMode(NEOPIXEL_I2C_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_I2C_POWER, HIGH);

  if (false == _display.begin(DISPLAY_ADDR, true)) {
    Log.errorln("display.begin");
    passed = false;
  } else {
    _display.setRotation(1);
    _display.setTextSize(1);
    _display.setTextColor(SSD1327_WHITE);
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
      }
      break;
    case AppState_t::DISP_CADENCE_STATE:
      if (true == blackboard.cadence.valid) {
        // Clear old data
        _display.clearDisplay();

        // Print cadence
        _display.setCursor(2, 2);
        _display.setTextSize(7);
        _display.print(blackboard.cadence.cadence);

        // Print line seps
        _display.drawFastHLine(0, 64, 128, SSD1327_WHITE);
        _display.drawFastHLine(0, 65, 128, SSD1327_WHITE);

        // Print distance in miles
        _display.setCursor(2, 80);
        _display.setTextSize(5);
        _display.print(blackboard.cadence.distance, 2);

        // Actually show it all
        _display.display();
      }
      break;
    case AppState_t::DISP_BLE_ABORT:
      _clear();
      _display.println("No device found");
      _display.display();
      next_state = AppState_t::BLE_ABORT;
      break;
    case AppState_t::BLE_ABORT:
      break;
  }

  _state_ticks++;

  _state = next_state;
}

void DisplayManager::_clear(void) {
  _display.clearDisplay();
  _display.display();
}
