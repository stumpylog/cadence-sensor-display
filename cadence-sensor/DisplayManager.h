
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

// Standard Libraries
#include <cstdint>

// Display Libraries
#include <Adafruit_SH110X.h>

class DisplayManager : public Adafruit_SH1107 {
  public:
    DisplayManager(void);
    ~DisplayManager(void);

    void initialize(void);

    void clear(void);

    void landscape(void);

    void position(int16_t const, int16_t const);

    void clear_lines(void);
    void insert_line(char const[]);
    void println_lines(void);

    void display_cadence(uint32_t const);

  private:
    // Display - I2C address
    static constexpr uint8_t DISPLAY_ADDR{ 0x3C };
    // Display - maximum lines at font size 1
    static constexpr uint8_t DISPLAY_MAX_LINES{ 8 };
    // Display - maximum characters at font size 1
    static constexpr uint8_t DISPLAY_MAX_CHARS_PER_LINE{ 20 };
    // Display - cadence font size
    static constexpr uint8_t CADENCE_FONT_SIZE{ 7 };
    static constexpr uint8_t CADENCE_FONT_CENTER_X{ 2 };
    static constexpr uint8_t CADENCE_FONT_CENTER_Y{ 5 };
    int8_t head_idx;
    int8_t tail_idx;
    char lines[DISPLAY_MAX_LINES][DISPLAY_MAX_CHARS_PER_LINE] = {{0}};
    bool cadence_setup;

};

#endif
