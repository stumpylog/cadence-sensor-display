
#include "Tasks.h"

#include "BluetoothManager.h"
#include "DisplayManager.h"

static BluetoothManager ble(scanCompleteCB, notifyCallback);
static DisplayManager disp;

IApplication* tasks[TASK_COUNT] = {
  &ble,
  &disp,
};
