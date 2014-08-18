#include "SchedulerRegistry.h"
#include <propeller.h>

SchedulerRegistry::SchedulerRegistry() {
  _schedulersInList = 0;
}

SchedulerRegistry::~SchedulerRegistry() {
  //dtor
}


void SchedulerRegistry::clear() {
  for (int i = 0; i <= MAX_SCHEDULERS; i++) {
    _pSchedulers[i] = 0;
  }
  _schedulersInList = 0;
}

void SchedulerRegistry::addScheduler (Scheduler &scheduler, uint16_t updateIntervalMs) {
  _pSchedulers[_schedulersInList++] = &scheduler;
  scheduler.setUpdateIntervalMs(updateIntervalMs);
}

HUBTEXT void SchedulerRegistry::schedule() {
  for (int i = 0; i < _schedulersInList; i++) {
    if (_pSchedulers[i]->shouldSchedule()) {
      _pSchedulers[i]->schedule();
    }
  }
}
