# README

- [README](#readme)
  - [Introduction](#introduction)
  - [Bill of Materials](#bill-of-materials)
  - [Case](#case)
  - [Software](#software)
    - [Display Driver](#display-driver)
    - [Board](#board)
    - [Bluetooth Low Energy](#bluetooth-low-energy)
    - [Logging](#logging)
  - [Other Reading](#other-reading)
    - [Bluetooth Standards](#bluetooth-standards)

## Introduction

This is a project to receive, calculate and display the cadence from a BLE device using the Cycling Speed and Cadence service.  It uses the below listed hardware and

## Bill of Materials

* [Adafruit HUZZAH32 – ESP32](https://www.adafruit.com/product/3405)
  * [Documentaton](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/overview)
* [Adafruit FeatherWing OLED - 128x64 OLED](https://www.adafruit.com/product/4650)
  * [Documentation](https://learn.adafruit.com/adafruit-128x64-oled-featherwing/overview)

## Case

See https://learn.adafruit.com/3d-printed-case-for-adafruit-feather/overview

See https://www.thingiverse.com/thing:2438577 and https://www.thingiverse.com/thing:2209964

* feather-case.stl
* feather-bat-switch.stl
* feather-top-wing.stl

Also see https://learn.adafruit.com/on-slash-off-switches/overview

## Software

### Display Driver

* [Adafruit_SH110x](https://github.com/adafruit/Adafruit_SH110x)
* [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)

### Board

* [espressif/arduino-esp32](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md)

### Bluetooth Low Energy

* [ESP32 BLE Arduino](https://www.arduino.cc/reference/en/libraries/esp32-ble-arduino/)

### Logging

* [Arduino-Log](https://github.com/thijse/Arduino-Log)

## Other Reading

* https://jmparound.blogspot.com/2021/02/building-cadence-display-for-ble.html
* https://github.com/jamesmontemagno/mycadence-arduino
* https://stackoverflow.com/questions/64624472/ble-cycling-speed-and-cadence-service-crank-timing-data

### Bluetooth Standards

* [CSC Service](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.cycling_speed_and_cadence.xml)
* [CSC Measurement Characteristic](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.csc_measurement.xml)
