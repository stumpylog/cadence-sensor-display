#pragma once

// Standard Libraries
#include <cstdint>
#include <cmath>

// Local
#include "IApplication.h"

class CadenceCalculator : public IApplication {
public:
  CadenceCalculator(void);
  ~CadenceCalculator(void);

  bool initialize(void);

  void step(void);

private:
  // Types
  enum class AppState_t : uint8_t {
    CALCULATE = 0,
  };

  // Constants
  // Sensor - staleness limit in milliseconds
  static constexpr uint16_t SENSOR_STALENESS_LIMIT_MS{ 2000 };
  // Sensor - last event time resolution
  static constexpr float_t SENSOR_TIME_RESOLUTION{ 1024.0f };
  static constexpr float_t SECONDS_PER_MINUTE{ 60.0f };
  static constexpr float_t WHEEL_CIRCUMFERENCE_INCHES{ 51.836f };
  static constexpr float_t INCHES_TO_MILES{ 0.000015782828283f };

  // Methods
  bool connect(void);

  // Fields
  // Current state
  AppState_t _state;

  // Sensor data
  uint16_t _prevCumlativeCranks;
  uint16_t _prevLastWheelEventTime;

  uint32_t _lastCalculateTime;
  uint64_t _sessionCranks;

  void _calculate(void);
};
