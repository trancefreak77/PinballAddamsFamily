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

    // Define all event notifications here!
    // void OnStartGamePressed(EventMessage &rMessage) {}
    void onBumper1SwitchClosed();
    void onBumper1SwitchOpened();
    void onBumper2SwitchClosed();
    void onBumper2SwitchOpened();
    void onBumper3SwitchClosed();
    void onBumper3SwitchOpened();
    void onBumper4SwitchClosed();
    void onBumper4SwitchOpened();
    void onBumper5SwitchClosed();
    void onBumper5SwitchOpened();
    void onLeftSlingshotSwitchClosed();
    void onLeftSlingshotLeftSwitchOpened();
    void onRightSlingshotSwitchClosed();
    void onRightSlingshotSwitchOpened();
    void onShooterLanePlungerSwitchClosed();
    void onShooterLanePlungerSwitchOpened();
    void onShooterLaneRampSwitchClosed();
    void onShooterLaneRampSwitchOpened();
    void onOuterLoopRightSwitchClosed();
    void onOuterLoopRightSwitchOpened();
    void onOuterLoopLeftSwitchClosed();
    void onOuterLoopLeftSwitchOpened();
    void onThingRampSwitchClosed();
    void onThingRampSwitchOpened();
    void onThingHoleSwitchClosed();
    void onThingHoleSwitchOpened();
    void onFarLeftInlaneSwitchClosed();
    void onFarLeftInlaneSwitchOpened();
    void onLeftOutlaneSwitchClosed();
    void onLeftOutlaneSwitchOpened();
    void onLeftInnerInlaneSwitchClosed();
    void onLeftInnerInlaneSwitchOpened();
    void onRightInlaneSwitchClosed();
    void onRightInlaneSwitchOpened();
    void onRightOutlaneSwitchClosed();
    void onRightOutlaneSwitchOpened();
    void onLeftFlipperButtonSwitchClosed();
    void onLeftFlipperButtonSwitchOpened();
    void onRightFlipperButtonSwitchClosed();
    void onRightFlipperButtonSwitchOpened();

  protected:
  private:
    StateBase *_pCurrentState;
    uint16_t _credits;          // Credit in cents.
};

#endif // PINBALLHSM_H
