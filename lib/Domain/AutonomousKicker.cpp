#include "AutonomousKicker.h"
#include <propeller.h>
#include "Timer\CounterSpan.h"

AutonomousKicker::AutonomousKicker(uint8_t &outputPortValue, uint8_t &inputPortValue, uint32_t activationTimeMs) :
  _outputActivator(outputPortValue, activationTimeMs), _oneTimeHitCounter(inputPortValue) {
  _startActivationCNT = 0;
}

AutonomousKicker::~AutonomousKicker() {
  //dtor
}

void AutonomousKicker::setNextActivationDeltaMs(uint16_t delta) {
  _nextActivationDeltaMs = delta;
  _deltaTicks = (CLKFREQ / 1000) * _nextActivationDeltaMs;
  _startActivationCNT = 0;
}

void AutonomousKicker::schedule() {
  if (_nextActivationDeltaMs > 0 && _startActivationCNT > 0) {
    if (counterDiff(_startActivationCNT, CNT) < _deltaTicks) {
      // Delta time not elapsed.
      _outputActivator.update();
      Scheduler::schedule();
      return;
    } else {
      _startActivationCNT = 0;
    }
  }

  if (_oneTimeHitCounter.isImpule()) {
    _outputActivator.activate();
    _startActivationCNT = CNT;
  }

  _outputActivator.update();
  Scheduler::schedule();
}



