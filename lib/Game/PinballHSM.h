#ifndef PINBALLHSM_H
#define PINBALLHSM_H
#include "Game\StateBase.h"
#include "Game\InOperationState.h"
#include "Domain\AutonomousKicker.h"
#include <stdint.h>

class PinballHSM {
  public:
    StateBase &getCurrentState();
    void setCurrentState(StateBase &state);

    // Define all state objects here.
    // InPlayState inPlayState;
    // InHitCousinItState inHitCousingItState;
    // ...
    // The states can be accessed and used by
    // the different state implementations to set
    // the _currentState.
    static InOperationState inOperationState;

    void init();

    void OnCoinInserted(uint16_t cents) {}
    void OnCoinReleased() {}
    void OnTargetHit(EventMessage &rMessage) {}
    void OnLeftFlipperButtonPressed(EventMessage &rMessage) {}
    void OnRightFlipperButtonPressed(EventMessage &rMessage) {}
    void OnBumperHit(EventMessage &rMessage) {}
    void OnSlingshotHit(EventMessage &rMessage) {}
    void OnScoopHit(EventMessage &rMessage) {}
    void OnThingHoleHit(EventMessage &rMessage) {}
    void OnStartGamePressed(EventMessage &rMessage) {}

  protected:
  private:
    void increaseCredit(uint16_t cents);
    void decreaseCredit(uint16_t cents);

    StateBase *_pCurrentState;
    uint16_t _credits;          // Credit in cents.
};

#endif // PINBALLHSM_H
