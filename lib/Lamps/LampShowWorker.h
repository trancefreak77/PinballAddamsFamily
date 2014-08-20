#ifndef LAMPSHOWWORKER_H
#define LAMPSHOWWORKER_H

#include "LampShow.h"
class LampShow;

class LampShowWorker {
  public:
    LampShowWorker(LampShow *pParentLampShow, bool shouldLoop);
    ~LampShowWorker();
    virtual void schedule();
    virtual bool getShouldLoop();
  protected:
    LampShow *_pParentLampShow;
  private:
    bool _shouldLoop;
};

#endif // LAMPSHOWWORKER_H
