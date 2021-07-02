#include "DisplayManager.h"

DisplayManager::DisplayManager() :
  Adafruit_SH1107(64, 128, &Wire)
{
  this->begin(DISPLAY_ADDR, true);
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

void DisplayManager::print(char const s[])
{
  Adafruit_SH1107::print(s);
  Adafruit_SH1107::display();
}

void DisplayManager::println(char const s[])
{
  // TODO - manage lines to simulate scrolling behavior
  Adafruit_SH1107::println(s);
  Adafruit_SH1107::display();
}
