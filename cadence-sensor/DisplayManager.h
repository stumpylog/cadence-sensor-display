
#ifndef DISPLAY_MANAGER
#define DISPLAY_MANAGER

// Standard Libraries
#include <stdint.h>

// Arduino Libraries
#include <Wire.h>
#include <Stream.h>

// Display Libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

class DisplayManager : public Adafruit_SH1107 {
  public:
    DisplayManager(void);
    ~DisplayManager(void);

    void splash(void);
    void clear(void);

    void landscape(void);

    void position(int16_t const, int16_t const);

    void print(const char[]);
    void println(const char[]);

  private:
    // Display - I2C address
    static constexpr uint8_t DISPLAY_ADDR{0x3C};
    // Display - maximum lines at font size 1
    // TODO - determine actual number
    static constexpr uint8_t DISPLAY_MAX_LINES{6};
};

#endif
