#ifndef DEBUG_SERIAL_H
#define DEBUG_SERIAL_H

#define DEBUG_LVL_NONE    0ul
#define DEBUG_LVL_ERR     1ul
#define DEBUG_LVL_WARN    2ul
#define DEBUG_LVL_INFO    3ul
#define DEBUG_LVL_VERBOSE 4ul

#define DEBUG_LVL DEBUG_LVL_INFO

#if defined(DEBUG_LVL) && (DEBUG_LVL > DEBUG_LVL_NONE)

#include <HardwareSerial.h>

#define DebugSerialStart(val)           Serial.begin(val)
#define DebugSerialReady()              while (!Serial) {}
#define DebugSerialPrint(val)           Serial.print(val)
#define DebugSerialPrintFmt(val, fmt)   Serial.print(val, fmt)
#define DebugSerialPrintLn(val)         Serial.println(val)
#define DebugSerialPrintLnFmt(val, fmt) Serial.println(val, fmt)
#define DebugSerialFlush()              Serial.flush()

#if (DEBUG_LVL >= DEBUG_LVL_VERBOSE)
#define DebugSerialVerbose(val)    DebugSerialPrintLn("DEBUG: " val)
#else
#define DebugSerialVerbose(val)
#endif

#if (DEBUG_LVL >= DEBUG_LVL_INFO)
#define DebugSerialInfo(val)    DebugSerialPrintLn("INFO: " val)
#else
#define DebugSerialInfo(val)
#endif

#if (DEBUG_LVL >= DEBUG_LVL_WARN)
#define DebugSerialWarn(val)    DebugSerialPrintLn("WARN: " val)
#else
#define DebugSerialWarn(val)
#endif

#if (DEBUG_LVL >= DEBUG_LVL_ERR)
#define DebugSerialErr(val)    DebugSerialPrintLn("ERR: " val)
#else
#define DebugSerialErr(val)
#endif

#else

#define DebugSerialStart(val)
#define DebugSerialReady()
#define DebugSerialPrint(val)
#define DebugSerialPrintLn(val)
#define DebugSerialFlush()

#define DebugSerialVerbose(val)
#define DebugSerialInfo(val)
#define DebugSerialWarn(val)
#define DebugSerialErr(val)

#endif

#endif
