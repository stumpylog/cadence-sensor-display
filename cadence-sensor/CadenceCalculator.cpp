#include "CadenceCalculator.h"

// Local
#include "LoggingConfig.h"
#include "Blackboard.h"

// Types

// Globals

CadenceCalculator::CadenceCalculator(void)
  : _state{ AppState_t::CALCULATE },
    _prevCumlativeCranks{ 0 },
    _prevLastWheelEventTime{ 0 },
    _lastCalculateTime{ 0 } {}

CadenceCalculator::~CadenceCalculator(void) {}

bool CadenceCalculator::initialize(void) {

  // Reset all state and counters
  _state = AppState_t::CALCULATE;
  _prevCumlativeCranks = 0;
  _prevLastWheelEventTime = 0;
  _lastCalculateTime = 0;

  return true;
}

void CadenceCalculator::step(void) {

  AppState_t nextState{ _state };
  switch (_state) {
    case AppState_t::CALCULATE:
      _calculate();
      break;
    default:
      break;
  }

  _state = nextState;
}

void CadenceCalculator::_calculate(void) {

  if (true == blackboard.ble.valid) {

      int32_t deltaRotations = blackboard.ble.cumlativeCranks - _prevCumlativeCranks;

      if (deltaRotations < 0) {
        // Roll over
        Log.noticeln("Rotations rollover");
        deltaRotations += 0xFFFF;
      }

      int32_t timeDelta = blackboard.ble.lastWheelEventTime - _prevLastWheelEventTime;
      if (timeDelta < 0) {
        // Roll over
        Log.noticeln("Time rollover");
        timeDelta += 0xFFFF;
      }

      if ((timeDelta != 0) && (deltaRotations != 0)) {
        // Convert event time delta to a time in minutes
        float const timeMins = static_cast<float>(timeDelta) / SENSOR_TIME_RESOLUTION / SECONDS_PER_MINUTE;
        // Calculate new RPM
        blackboard.cadence.cadence = static_cast<uint16_t>(static_cast<float>(deltaRotations) / timeMins);
        _lastCalculateTime = millis();
        Log.noticeln("Cadence %d", blackboard.cadence.cadence);
        // Save latest data
        _prevCumlativeCranks = blackboard.ble.cumlativeCranks;
        _prevLastWheelEventTime = blackboard.ble.lastWheelEventTime ;
      } else {
        uint32_t const calculate_delta = millis() - _lastCalculateTime;
        if (calculate_delta > SENSOR_STALENESS_LIMIT_MS) {
          blackboard.cadence.cadence = 0;
        }
        Log.noticeln("Calculate Delta %d", calculate_delta);
      }

    blackboard.cadence.valid = true;

  } else {
    blackboard.cadence.valid = false;
  }
}
