
#include "Tasks.h"

#include "BluetoothManager.h"
#include "DisplayManager.h"
#include "CadenceCalculator.h"

static BluetoothManager ble;
static DisplayManager disp;
static CadenceCalculator calc;

IApplication* tasks[TASK_COUNT] = {
  &ble,
  &calc,
  &disp,
};
