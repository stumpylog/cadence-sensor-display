#include "DisplayManager.h"

// Arduino Libraries
#include <Wire.h>

#include <cstring>

DisplayManager::DisplayManager(void) :
  Adafruit_SH1107(64, 128, &Wire),
  head_idx{ -1},
  tail_idx{ -1}
{
  this->begin(DISPLAY_ADDR, true);
  this->landscape();
  this->setTextSize(1);
  this->setTextColor(SH110X_WHITE);
  this->position(0, 0);
}

DisplayManager::~DisplayManager(void) {}

void DisplayManager::splash(void)
{
  this->display();
  delay(1000);
}

void DisplayManager::clear(void)
{
  this->clearDisplay();
  this->display();
}

void DisplayManager::landscape(void)
{
  this->setRotation(1);
}

void DisplayManager::position(int16_t const x,
                              int16_t const y)
{
  this->setCursor(x, y);
}

void DisplayManager::clear_lines(void)
{
  this->head_idx = -1;
  this->tail_idx = -1;
}

void DisplayManager::insert_line(char const s[])
{
  if (this->head_idx == -1)
  {
    this->head_idx = 0;
    this->tail_idx = 0;
  }
  else
  {
    this->tail_idx = (this->tail_idx + 1) % DISPLAY_MAX_LINES;
    if (this->tail_idx <= this->head_idx)
    {
      this->head_idx = (this->head_idx + 1) % DISPLAY_MAX_LINES;
    }
  }
  strncpy(this->lines[tail_idx], s, DISPLAY_MAX_CHARS_PER_LINE);
}

void DisplayManager::println_lines(void)
{
  if (this->head_idx != -1)
  {
    if (this->tail_idx >= this->head_idx)
    {
      for (int8_t i = this->head_idx; i < (this->tail_idx + 1); i++)
      {
        Adafruit_SH1107::println(lines[i]);
      }
    }
    else {
      for (int8_t i = this->head_idx; i < DISPLAY_MAX_LINES; i++)
      {
        Adafruit_SH1107::println(lines[i]);
      }
      for (int8_t i = 0; i < (this->tail_idx + 1); i++)
      {
        Adafruit_SH1107::println(lines[i]);
      }
    }
  }
}
