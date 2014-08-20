#ifndef LAMPSHOW_H
#define LAMPSHOW_H

#include "Domain\Scheduler.h"
#include "LampShowWorker.h"
class LampShowWorker;

class LampShow : public Scheduler {
  public:
    enum class Sequence {
      BallPlunged,
      Multiball,
      Jackpot,
      SuperJackpot
    };

    LampShow(uint8_t lampState[], uint8_t outputPort[]);
    ~LampShow();
    void schedule();
    void playLampShow(Sequence sequence);
    void finishedLampShow(LampShowWorker *lampShowWorker);

    uint8_t *getLampStateArray();
    uint8_t *getOutputPortArray();
  protected:
  private:
    LampShowWorker *_pWorkerArray[3];
    uint8_t *_pLampState;
    uint8_t *_pOutputPort;
};

#endif // LAMPSHOW_H
