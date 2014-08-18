#ifndef STATEBASE_H
#define STATEBASE_H
#include "EventMessage.h"
#include <stdint.h>

class PinballHSM;

class StateBase {
  public:
    virtual void OnCoinInserted(PinballHSM &rHsm, uint16_t cents) {}
    virtual void OnCoinReleased(PinballHSM &rHsm) {}
    virtual void OnTargetHit(PinballHSM &rHsm, EventMessage &rMessage) {}
    virtual void OnLeftFlipperButtonPressed(PinballHSM &rHsm, EventMessage &rMessage) {}
    virtual void OnRightFlipperButtonPressed(PinballHSM &rHsm, EventMessage &rMessage) {}
    virtual void OnBumperHit(PinballHSM &rHsm, EventMessage &rMessage) {}
    virtual void OnSlingshotHit(PinballHSM &rHsm, EventMessage &rMessage) {}
    virtual void OnScoopHit(PinballHSM &rHsm, EventMessage &rMessage) {}
    virtual void OnThingHoleHit(PinballHSM &rHsm, EventMessage &rMessage) {}
    virtual void OnStartGamePressed(PinballHSM &rHsm, EventMessage &rMessage) {}
  protected:
  private:
};

#endif // STATEBASE_H
