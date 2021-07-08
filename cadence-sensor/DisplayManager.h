
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

    void splash(void);
    void clear(void);

    void landscape(void);

    void position(int16_t const, int16_t const);

    void clear_lines(void);
    void insert_line(char const[]);
    void println_lines(void);

  private:
    // Display - I2C address
    static constexpr uint8_t DISPLAY_ADDR{0x3C};
    // Display - maximum lines at font size 1
    // TODO - determine actual number
    static constexpr uint8_t DISPLAY_MAX_LINES{6};
    static constexpr uint8_t DISPLAY_MAX_CHARS_PER_LINE{255};
    int8_t head_idx;
    int8_t tail_idx;
    char lines[DISPLAY_MAX_LINES][DISPLAY_MAX_CHARS_PER_LINE] = {{0}};

};

#endif
