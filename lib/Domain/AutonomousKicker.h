#ifndef AUTONOMOUSKICKER_H
#define AUTONOMOUSKICKER_H

#include "Scheduler.h"
#include "OutputActivator.h"
#include "OneTimeHitCounter.h"


class AutonomousKicker : public Scheduler {
  public:
    AutonomousKicker(uint8_t &outputPortValue, uint8_t &inputPortValue, uint32_t activationTimeMs);
    virtual void schedule();
    virtual void setNextActivationDeltaMs(uint16_t delta);
    virtual ~AutonomousKicker();

  protected:
  private:
    OutputActivator _outputActivator;
    OneTimeHitCounter _oneTimeHitCounter;
    uint16_t _nextActivationDeltaMs;
    uint32_t _deltaTicks;
    uint32_t _startActivationCNT;
};

#endif // AUTONOMOUSKICKER_H
