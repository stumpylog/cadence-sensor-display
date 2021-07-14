# README

- [README](#readme)
  - [Introduction](#introduction)
  - [Bill of Materials](#bill-of-materials)
  - [Software](#software)
    - [Display Driver](#display-driver)
    - [Board](#board)
    - [Bluetooth Low Energy](#bluetooth-low-energy)
  - [Other Reading](#other-reading)
    - [Bluetooth Standards](#bluetooth-standards)
  - [Design](#design)
    - [Initialization](#initialization)
    - [State Flow](#state-flow)
      - [SCAN_DEVICES](#scan_devices)
      - [SCAN_RUNNING](#scan_running)
      - [CONNECT_TO_SENSOR](#connect_to_sensor)
      - [DISPLAY_CADENCE](#display_cadence)
      - [SENSOR_DISCONNECT](#sensor_disconnect)
      - [ABORT_NOTIFY](#abort_notify)
      - [ABORT](#abort)

## Introduction

This is a project to receive, calculate and display the cadence from a BLE device using the Cycling Speed and Cadence service.  It uses the below listed hardware and

## Bill of Materials

* [Adafruit HUZZAH32 – ESP32](https://www.adafruit.com/product/3405)
  * [Documentaton](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/overview)
* [Adafruit FeatherWing OLED - 128x64 OLED](https://www.adafruit.com/product/4650)
  * [Documentation](https://learn.adafruit.com/adafruit-128x64-oled-featherwing/overview)

## Software

### Display Driver

* [Adafruit_SH110x](https://github.com/adafruit/Adafruit_SH110x)
* [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)


### Board

* [espressif/arduino-esp32](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md)

### Bluetooth Low Energy

* [ESP32 BLE Arduino](https://www.arduino.cc/reference/en/libraries/esp32-ble-arduino/)

## Other Reading

* https://jmparound.blogspot.com/2021/02/building-cadence-display-for-ble.html
* https://github.com/jamesmontemagno/mycadence-arduino
* https://stackoverflow.com/questions/64624472/ble-cycling-speed-and-cadence-service-crank-timing-data

### Bluetooth Standards

* [CSC Service](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.cycling_speed_and_cadence.xml)
* [CSC Measurement Characteristic](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.csc_measurement.xml)

## Design

The software is implemented as a state machine.  During `setup`, the application's `initialize` method is called.  After setup, `loop` calls the applications `step`, where all state and logic will be executed.

### Initialization

During the `initialize`, the application will set up the display, connecting to it over I2C using the default address.  Next, the BLE stack is initialized and certain scan settings are set, including registering the class as the callback for `onResult` of a BLE advertised device being found.


### State Flow

![](img/cadence-state.png)

#### SCAN_DEVICES

The first order of business is to locate the cadence sensor.  During `SCAN_DEVICES` the BLE scan is started with a callback registered for the completion of the scan.

If the scan has happened 10 or fewer times, the state will transition to `SCAN_RUNNING`.

If the scan has happened 11 or more times, the state will transition to `ABORT_NOTIFY`.

#### SCAN_RUNNING

During `SCAN_RUNNING`, the application only outputs some information to the serial interface.

If no devices advertising the CSC service are found during this scan, the state will transition back to `SCAN_DEVICES`.

If a device is found advertising the CSC service, the `onResult` method will transition the state to `CONNECT_TO_SENSOR`.

#### CONNECT_TO_SENSOR

During `CONNECT_TO_SENSOR`, a remote device is connected to and the notify characteristic is set with the notify callback.

If connection fails, the state will transition to `SCAN_DEVICES`.

If the connection succeeds, the state will transition to `DISPLAY_CADENCE`.

#### DISPLAY_CADENCE

During `DISPLAY_CADENCE`, the cadence is displayed, if it has changed.  The cadence is calculated by the notify callback, as sent by the sensor when the characteristic changes.

If the `onDisconnect` callback is called, the state will transition to `SENSOR_DISCONNECT`.

Otherwise, the application is expected to remain in this state until power off.

#### SENSOR_DISCONNECT

During `SENSOR_DISCONNECT`, a serial message is sent to notify the sensor has disconnected.

The state will transition to `CONNECT_TO_SENSOR`.

#### ABORT_NOTIFY

The state will transition to `ABORT`.

#### ABORT

The state will never transition.
