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

/** @brief Decreases the current credit by given amount.
  *
  * Decreases the current credit by given amount.
  */
void PinballHSM::decreaseCredit(uint16_t cents) {
  _credits -= cents;
}

/** @brief Increases the current credit by given amount.
  *
  * Increases the current credit by given amount.
  */
void PinballHSM::increaseCredit(uint16_t cents) {
  _credits += cents;
}

