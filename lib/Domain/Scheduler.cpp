#include "Scheduler.h"
#include "../Timer/CounterSpan.h"
#include <propeller.h>

Scheduler::Scheduler() {
  _lastScheduledCNT = 0;
}

Scheduler::~Scheduler() {}

bool Scheduler::shouldSchedule() {
  if (_lastScheduledCNT == 0) {
    _lastScheduledCNT = CNT;
    return true;
  }

  if (counterDiff(_lastScheduledCNT, CNT) > _updateIntervalTicks) {
    return true;
  }

  return false;
}

void Scheduler::schedule() {
  _lastScheduledCNT = CNT;

  // Specific implementation must be done in
  // super class.
}

uint16_t Scheduler::getUpdateIntervalMs() {
  return _updateIntervalMs;
}

void Scheduler::setUpdateIntervalMs(uint16_t updateIntervalMs) {
  _updateIntervalMs = updateIntervalMs;
  _updateIntervalTicks = (CLKFREQ / 1000) * updateIntervalMs;
}
