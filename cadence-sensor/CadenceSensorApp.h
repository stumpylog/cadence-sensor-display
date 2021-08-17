#ifndef CADENCE_SENSOR_APP_H
#define CADENCE_SENSOR_APP_H

// Standard Libraries
#include <cstdint>
#include <cmath>

// Local
#include "IApplication.h"

class CadenceSensorApp : public IApplication {
  public:
    CadenceSensorApp(void);
    ~CadenceSensorApp(void);

    bool initialize(void);

    void step(void);

  private:
    // Types
    enum class AppState_t : uint8_t {
      SCAN_DEVICES = 0,
    };

    // Constants
    // Sensor - staleness cycles
    static constexpr uint16_t SENSOR_STALENESS_LIMIT_MS{ 2000 };
    // Sensor - last event time resolution
    static constexpr float_t SENSOR_TIME_RESOLUTION{ 1024.0f };
    static constexpr float_t SECONDS_PER_MINUTE{ 60.0f };

    // Methods
    bool connect(void);

    // Fields
    // Current state
    AppState_t _state;


    // Sensor data
    uint16_t _prevCumlativeCranks;
    uint16_t _prevLastWheelEventTime;

    void calculate(void);
};

#endif
