#ifndef INOPERATIONSTATE_H
#define INOPERATIONSTATE_H
#include "StateBase.h"
class PinballHSM;

class InOperationState : public StateBase {
  public:
    virtual void OnCoinInserted(PinballHSM &rHsm, uint16_t cents);
    virtual void OnCoinReleased(PinballHSM &rHsm);
  protected:
  private:
};

#endif // INOPERATIONSTATE_H
