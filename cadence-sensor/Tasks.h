#pragma once

#include <cstdint>

#include "IApplication.h"

enum Tasks_t : uint8_t {
  BLE_TASK_IDX = 0,
  CALC_TASK_IDX,
  DISP_TASK_IDX,
  TASK_COUNT
};

extern IApplication* tasks[Tasks_t::TASK_COUNT];
