#pragma once

// Standard Libraries
#include <cstdint>

// Display Libraries
#include <Adafruit_SH110X.h>

// Local
#include "IApplication.h"

class DisplayManager : public IApplication {
public:
  DisplayManager(void);
  ~DisplayManager(void);

  bool initialize(void);

  void step(void);

private:
  void _clear(void);

  // Types
  enum class AppState_t : uint8_t {
    DISP_VERSION_STATE = 0,
    VERSION_TRANSITION,
    DISP_VOLTAGE_STATE,
    VOLTAGE_TRANSITION,
    DISP_NO_CADENCE,
    WAIT_CADENCE,
    DISP_BLE_ABORT,
    BLE_ABORT,
    CADENCE_SETUP,
    DISP_CADENCE_STATE,
    DISP_SLEEPING,
    SLEEP
  };

  // Constants
  // Display - I2C address
  static constexpr uint8_t DISPLAY_ADDR{ 0x3C };
  // Display - width
  static constexpr uint8_t DISPLAY_WIDTH{ 128 };
  // Display - height
  static constexpr uint8_t DISPLAY_HEIGHT{ 64 };
  // Display - maximum lines at font size 1
  static constexpr uint8_t DISPLAY_MAX_LINES{ 8 };
  // Display - maximum characters at font size 1
  static constexpr uint8_t DISPLAY_MAX_CHARS_PER_LINE{ 20 };
  // Display - cadence font size
  static constexpr uint8_t CADENCE_FONT_SIZE{ 4 };
  static constexpr uint8_t CADENCE_FONT_CENTER_X{ 0 };
  static constexpr uint8_t CADENCE_FONT_CENTER_Y{ 20 };
  // Display - distance font and location
  static constexpr uint8_t DISTANCE_FONT_SIZE{ 2 };
  static constexpr uint8_t DISTANCE_FONT_CENTER_X{ 80 };
  static constexpr uint8_t DISTANCE_FONT_CENTER_Y{ 28 };

  static constexpr uint8_t VERSION_DISPLAY_TICKS{ 20 };
  static constexpr uint8_t POWER_DISPLAY_TICKS{ 50 };

  // Vars
  Adafruit_SH1107 _display;
  AppState_t _state;
  uint8_t _state_ticks;
};
