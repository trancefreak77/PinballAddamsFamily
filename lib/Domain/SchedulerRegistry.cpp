#include "SchedulerRegistry.h"
#include <propeller.h>
#include <stdint.h>

SchedulerRegistry::SchedulerRegistry() {
  _schedulersInList = 0;
}

SchedulerRegistry::~SchedulerRegistry() {
  //dtor
}


void SchedulerRegistry::clear() {
  for (uint8_t i = 0; i <= MAX_SCHEDULERS; i++) {
    _pSchedulers[i] = 0;
  }
  _schedulersInList = 0;
}

void SchedulerRegistry::addScheduler (Scheduler &scheduler, uint16_t updateIntervalMs) {
  _pSchedulers[_schedulersInList++] = &scheduler;
  scheduler.setUpdateIntervalMs(updateIntervalMs);
}

void SchedulerRegistry::schedule() {
  for (uint8_t i = 0; i < _schedulersInList; i++) {
    if (_pSchedulers[i]->shouldSchedule()) {
      _pSchedulers[i]->schedule();
    }
  }
}

