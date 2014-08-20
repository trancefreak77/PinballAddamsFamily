#include "PauseTimer.h"
#include "CounterSpan.h"
#include <propeller.h>

PauseTimer::PauseTimer() {
  _intervalMs = 0;
}

PauseTimer::~PauseTimer() {
  //dtor
}

void PauseTimer::start() {
  _startCnt = CNT;
}

bool PauseTimer::isElapsed(uint16_t intervalMs) {
  if (counterDiff(_startCnt, CNT) > msToCounterSpan(intervalMs)) {
    return true;
  }

  return false;
}
