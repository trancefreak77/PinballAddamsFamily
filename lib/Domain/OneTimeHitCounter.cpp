#include "OneTimeHitCounter.h"

OneTimeHitCounter::OneTimeHitCounter(uint8_t &inputPortValue) : _inputPortValue(inputPortValue) {
  _isActive = false;
}

bool OneTimeHitCounter::isImpule() {
  if (_inputPortValue == 1 && _isActive == false) {
    // Impulse present, set isActive = true
    _isActive = true;
    return true;
  }

  if (_inputPortValue == 0) {
    _isActive = false;
  }

  return false;
}
