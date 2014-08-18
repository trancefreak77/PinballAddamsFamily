#include "OutputActivator.h"
#include "../Timer/CounterSpan.h"
#include <propeller.h>

OutputActivator::OutputActivator(uint8_t &outputPortValue, uint32_t activationTimeMs) : _outputPortValue(outputPortValue) {
  setActivationTimeMs(activationTimeMs);
}

void OutputActivator::setActivationTimeMs(uint32_t activationTimeMs) {
  // Calculate the activation counter tick count.
  _activeCounterTicks = (CLKFREQ / 1000) * activationTimeMs;
  _activationTimeMs = activationTimeMs;
}

uint32_t OutputActivator::getActivationTimeMs() {
  return _activationTimeMs;
}

void OutputActivator::activate() {
  if (_isActive) {
    // Output is already active, do nothing...
    return;
  }

  _startCounterValue = CNT;
  _isActive = true;
  _outputPortValue = 1;
}

void OutputActivator::update() {
  if (!_isActive) {
    _outputPortValue = 0;
    return;
  }

  // The output is activated. Check if the
  // activation time is over...
  if (counterDiff(_startCounterValue, CNT) > _activeCounterTicks) {
    _isActive = false;
    _outputPortValue = 0;
  }
}
