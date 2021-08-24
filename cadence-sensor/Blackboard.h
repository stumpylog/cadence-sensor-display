#include <cstdint>

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
} CadenceCalculatorData;

typedef struct {
  bool valid;
  bool sleep;
  uint8_t percent;
} PowerData;

typedef struct {
  bool valid;
  uint16_t lastDisplayedCadence;
} DisplayData;

typedef struct {
  SystemData system;
  BluetoothCadenceData ble;
  CadenceCalculatorData cadence;
  PowerData power;
  DisplayData display;
} Blackboard;

extern Blackboard blackboard;
