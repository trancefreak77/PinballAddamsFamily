#ifndef LAMPSHOWWORKER_H
#define LAMPSHOWWORKER_H

#include "LampShow.h"
class LampShow;

class LampShowWorker {
  public:
    LampShowWorker();
    ~LampShowWorker();
    virtual void schedule();
  protected:
  private:
    LampShow *pParentLampShow;
};

#endif // LAMPSHOWWORKER_H
