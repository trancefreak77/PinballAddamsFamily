#ifndef SCHEDULERREGISTRY_H
#define SCHEDULERREGISTRY_H
#include "Scheduler.h"
#define MAX_SCHEDULERS 50

class SchedulerRegistry {
  public:
    SchedulerRegistry();
    ~SchedulerRegistry();
    void schedule();
    void addScheduler(Scheduler &scheduler, uint16_t updateIntervalMs);
    void clear();
  protected:
  private:
    Scheduler *_pSchedulers[MAX_SCHEDULERS];
    uint8_t _schedulersInList;
};

#endif // SCHEDULERREGISTRY_H
