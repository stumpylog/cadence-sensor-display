
#include "Tasks.h"

#include "BluetoothManager.h"
#include "DisplayManager.h"
#include "CadenceCalculator.h"
#include "PowerManager.h"

static BluetoothManager ble;
static DisplayManager disp;
static CadenceCalculator calc;
static PowerManager power;

IApplication* tasks[TASK_COUNT] = {
  &ble,
  &calc,
  &disp,
  &power,
};
