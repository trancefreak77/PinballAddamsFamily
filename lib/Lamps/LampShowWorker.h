#ifndef LAMPSHOWWORKER_H
#define LAMPSHOWWORKER_H

#include "LampShow.h"
class LampShow;

class LampShowWorker {
  public:
    LampShowWorker(LampShow *pParentLampShow);
    ~LampShowWorker();
    virtual void schedule();
  protected:
    LampShow *_pParentLampShow;
  private:
};

#endif // LAMPSHOWWORKER_H
