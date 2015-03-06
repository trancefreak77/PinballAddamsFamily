#ifndef LAMPSHOWWORKER_H
#define LAMPSHOWWORKER_H

#include "LampShow.h"
#include <stdint.h>
#include <map>

class LampShow;

class LampShowWorker {
  public:
    LampShowWorker(LampShow *pParentLampShow, bool shouldLoop);
    ~LampShowWorker();
    virtual void schedule();
    virtual bool getShouldLoop();
    std::map<uint8_t, bool>& getUnusedLampNumbers();
  protected:
    LampShow *_pParentLampShow;
    std::map<uint8_t, bool> _unusedLampsMap;
  private:
    bool _shouldLoop;
};

#endif // LAMPSHOWWORKER_H
