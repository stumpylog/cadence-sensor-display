#ifndef I_APPLICATION_H
#define I_APPLICATION_H

class IApplication {
  public:
    virtual bool initialize(void) = 0;
    virtual void step(void) = 0;
};

#endif
