#include "PinballHSM.h"
#include "Game\StateBase.h"

// Sets the current state.
void PinballHSM::setCurrentState(StateBase &state) {
  *_pCurrentState = state;
}

// Returns a reference to the current state.
StateBase &PinballHSM::getCurrentState() {
  return *_pCurrentState;
}

void PinballHSM::init() {
  this->setCurrentState(inOperationState);
}

void PinballHSM::onBumper1SwitchClosed() {}

void PinballHSM::onBumper1SwitchOpened() {}

void PinballHSM::onBumper2SwitchClosed() {}

void PinballHSM::onBumper2SwitchOpened() {}

void PinballHSM::onBumper3SwitchClosed() {}

void PinballHSM::onBumper3SwitchOpened() {}

void PinballHSM::onBumper4SwitchClosed() {}

void PinballHSM::onBumper4SwitchOpened() {}

void PinballHSM::onBumper5SwitchClosed() {}

void PinballHSM::onBumper5SwitchOpened() {}

void PinballHSM::onLeftSlingshotSwitchClosed() {}

void PinballHSM::onLeftSlingshotLeftSwitchOpened() {}

void PinballHSM::onRightSlingshotSwitchClosed() {}

void PinballHSM::onRightSlingshotSwitchOpened() {}

void PinballHSM::onShooterLanePlungerSwitchClosed() {}

void PinballHSM::onShooterLanePlungerSwitchOpened() {}

void PinballHSM::onShooterLaneRampSwitchClosed() {}

void PinballHSM::onShooterLaneRampSwitchOpened() {}

void PinballHSM::onOuterLoopRightSwitchClosed() {}

void PinballHSM::onOuterLoopRightSwitchOpened() {}

void PinballHSM::onOuterLoopLeftSwitchClosed() {}

void PinballHSM::onOuterLoopLeftSwitchOpened() {}

void PinballHSM::onThingRampSwitchClosed() {}

void PinballHSM::onThingRampSwitchOpened() {}

void PinballHSM::onThingHoleSwitchClosed() {}

void PinballHSM::onThingHoleSwitchOpened() {}

void PinballHSM::onFarLeftInlaneSwitchClosed() {}

void PinballHSM::onFarLeftInlaneSwitchOpened() {}

void PinballHSM::onLeftOutlaneSwitchClosed() {}

void PinballHSM::onLeftOutlaneSwitchOpened() {}

void PinballHSM::onLeftInnerInlaneSwitchClosed() {}

void PinballHSM::onLeftInnerInlaneSwitchOpened() {}

void PinballHSM::onRightInlaneSwitchClosed() {}

void PinballHSM::onRightInlaneSwitchOpened() {}

void PinballHSM::onRightOutlaneSwitchClosed() {}

void PinballHSM::onRightOutlaneSwitchOpened() {}

void PinballHSM::onLeftFlipperButtonSwitchClosed() {}

void PinballHSM::onLeftFlipperButtonSwitchOpened() {}

void PinballHSM::onRightFlipperButtonSwitchClosed() {}

void PinballHSM::onRightFlipperButtonSwitchOpened() {}
