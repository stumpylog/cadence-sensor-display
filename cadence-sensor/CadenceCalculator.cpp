#include "CadenceCalculator.h"

// Local
#include "LoggingConfig.h"
#include "Blackboard.h"

// Types

// Globals

CadenceCalculator::CadenceCalculator(void)
  : _state{ AppState_t::CALCULATE },
    _prevCumlativeCranks{ 0 },
    _prevLastWheelEventTime{ 0 } {}

CadenceCalculator::~CadenceCalculator(void) {}

bool CadenceCalculator::initialize(void) {

  // Reset all state and counters
  _state = AppState_t::CALCULATE;
  _prevCumlativeCranks = 0;
  _prevLastWheelEventTime = 0;

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

    uint32_t const last_update_time = (millis() - blackboard.ble.lastNotifyTime);

    if (last_update_time <= SENSOR_STALENESS_LIMIT_MS) {

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

      if (timeDelta != 0) {
        // Convert event time delta to a time in minutes
        float const timeMins = static_cast<float>(timeDelta) / SENSOR_TIME_RESOLUTION / SECONDS_PER_MINUTE;
        // Calculate new RPM
        blackboard.cadence.cadence = static_cast<uint16_t>(static_cast<float>(deltaRotations) / timeMins);
        Log.noticeln("Cadence %d", blackboard.cadence.cadence);
        // Save latest data
        _prevCumlativeCranks = blackboard.ble.cumlativeCranks;
        _prevLastWheelEventTime = blackboard.ble.lastWheelEventTime ;
      } else {
        Log.noticeln("Time delta %d", timeDelta);
      }

    } else {
      Log.warningln("Last notify time stale");
      blackboard.cadence.cadence = 0;
    }

    blackboard.cadence.valid = true;

  } else {
    blackboard.cadence.valid = false;
  }
}
