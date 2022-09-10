
#include <Arduino.h>

#include "PowerManager.h"
#include "Blackboard.h"
#include "LoggingConfig.h"

PowerManager::PowerManager(void) :
  _state{ AppState_t::READ_BATTERY },
  sleep_wait_cycles{ 0 } {}

PowerManager::~PowerManager(void) {}

bool PowerManager::initialize(void) {

    _state = AppState_t::READ_BATTERY;
    sleep_wait_cycles = 0;

    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_C, INPUT_PULLUP);

    esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(BUTTON_A), 0);

    return true;
}

void PowerManager::step(void) {

    AppState_t next_state { _state };

    switch (_state) {
        case AppState_t::READ_BATTERY:
            // Nothing to do here yet
            next_state = AppState_t::SLEEP_CHECK;
            break;
        case AppState_t::SLEEP_CHECK:
            if ((LOW == digitalRead(BUTTON_C)) || true == blackboard.ble.aborted) {
                next_state = AppState_t::SLEEP_NEXT;
                blackboard.power.sleep = true;
            }
            break;
        case AppState_t::SLEEP_NEXT:
            if (sleep_wait_cycles < 10) {
                sleep_wait_cycles++;
            } else {
                next_state = AppState_t::SLEEP;
            }
            break;
        case AppState_t::SLEEP:
            esp_deep_sleep_start();
            break;
    }

    _state = next_state;

}
