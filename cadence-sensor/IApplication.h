#pragma once

class IApplication {
public:
  virtual bool initialize(void) = 0;
  virtual void step(void) = 0;
};
