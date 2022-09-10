#pragma once

#include <cstdint>

#include "IApplication.h"


class PowerManager : public IApplication {
public:
  PowerManager(void);
  ~PowerManager(void);

  bool initialize(void);

  void step(void);

private:
      // Types
    enum class AppState_t : uint8_t {
      READ_BATTERY = 0,
      SLEEP_CHECK,
      SLEEP_NEXT,
      SLEEP
    };


    AppState_t _state;
    uint8_t sleep_wait_cycles;

    // Constants
    // Display - Buttons
    static constexpr uint8_t BUTTON_A{ 15 };
    static constexpr uint8_t BUTTON_B{ 32 };
    static constexpr uint8_t BUTTON_C{ 14 };

};
