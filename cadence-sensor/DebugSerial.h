#ifndef DEBUG_SERIAL
#define DEBUG_SERIAL

#define DEBUG_LVL_NONE    0ul
#define DEBUG_LVL_ERR     1ul
#define DEBUG_LVL_WARN    2ul
#define DEBUG_LVL_INFO    3ul
#define DEBUG_LVL_VERBOSE 4ul

#define DEBUG_LVL DEBUG_LVL_INFO

#if defined(DEBUG_LVL) && (DEBUG_LVL > DEBUG_LVL_NONE)

  #define DebugSerialStart(x)      Serial.begin(x)
  #define DebugSerialReady()       while (!Serial) {}
  #define DebugSerialPrint(x)      Serial.print(x)
  #define DebugSerialPrintLn(x)    Serial.println(x)
  #define DebugSerialFlush()       Serial.flush()

#if (DEBUG_LVL >= DEBUG_LVL_VERBOSE)
  #define DebugSerialVerbose(x)    DebugSerialPrintLn("DEBUG: " x)
#endif

#if (DEBUG_LVL >= DEBUG_LVL_INFO)
  #define DebugSerialInfo(x)    DebugSerialPrintLn("INFO: " x)
#endif

#if (DEBUG_LVL >= DEBUG_LVL_WARN)
  #define DebugSerialWarn(x)    DebugSerialPrintLn("WARN: " x)
#endif

#if (DEBUG_LVL >= DEBUG_LVL_ERR)
  #define DebugSerialErr(x)    DebugSerialPrintLn("ERR: " x)
#endif

#else

  #define DebugSerialStart(x)
  #define DebugSerialReady()
  #define DebugSerialPrint(x)
  #define DebugSerialPrintLn(x)
  #define DebugSerialFlush()

  #define DebugSerialVerbose(x)
  #define DebugSerialInfo(x)
  #define DebugSerialWarn(x)

#endif

#endif
