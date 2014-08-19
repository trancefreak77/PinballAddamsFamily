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

    LampShow();
    ~LampShow();
    void schedule();
    void playLampShow(Sequence sequence);
    void finishedLampShow();
  protected:
  private:
    LampShowWorker *_pWorkerArray[3];
};

#endif // LAMPSHOW_H
