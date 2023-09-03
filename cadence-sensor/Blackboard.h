#include <cstdint>
#include <cmath>

#pragma once

typedef struct {
  uint64_t cycle_counter;
} SystemData;

typedef struct {
  bool valid;
  bool aborted;
  uint16_t cumlativeCranks;
  uint16_t lastWheelEventTime;
} BluetoothCadenceData;

typedef struct {
  bool valid;
  uint16_t cadence;
  float_t distance;
} CadenceCalculatorData;

typedef struct {
  bool valid;
  uint16_t lastDisplayedCadence;
} DisplayData;

typedef struct {
  SystemData system;
  BluetoothCadenceData ble;
  CadenceCalculatorData cadence;
  DisplayData display;
} Blackboard;

extern Blackboard blackboard;
