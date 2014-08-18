#ifndef ISCHEDULABLE_H
#define ISCHEDULABLE_H
#include <stdint.h>
#include "ModelBase.h"

class Scheduler : public ModelBase {
  public:
    /** Default destructor */
    Scheduler();
    ~Scheduler();
    virtual void schedule();
    virtual bool shouldSchedule();
    uint16_t getUpdateIntervalMs();
    void setUpdateIntervalMs(uint16_t updateIntervalMs);
  protected:
  private:
    uint32_t _lastScheduledCNT;
    uint32_t _updateIntervalTicks;
    uint16_t _updateIntervalMs;
};

#endif // ISCHEDULABLE_H
