#ifndef MULTIBALLLAMPSHOWWORKER_H
#define MULTIBALLLAMPSHOWWORKER_H

#include "LampShowWorker.h"

class MultiballLampShowWorker : public LampShowWorker
{
  public:
    MultiballLampShowWorker(LampShow *pParentLampShow);
    virtual ~MultiballLampShowWorker();
    virtual void schedule();
  protected:
  private:
    int _currentState;
};

#endif // MULTIBALLLAMPSHOWWORKER_H
