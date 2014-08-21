#ifndef STATEBASE_H
#define STATEBASE_H
#include "EventMessage.h"
#include <stdint.h>

class PinballHSM;

class StateBase {
  public:
    // Define all notification events.
    // Each state has to override it's own implementation.
    virtual void OnCoinInserted(PinballHSM &rHsm, uint16_t cents) {}
    virtual void OnCoinReleased(PinballHSM &rHsm) {}
    // virtual void OnTargetHit(PinballHSM &rHsm, EventMessage &rMessage) {}
  protected:
  private:
};

#endif // STATEBASE_H
