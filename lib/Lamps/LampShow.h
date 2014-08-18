#ifndef LAMPSHOW_H
#define LAMPSHOW_H

#include "Domain\Scheduler.h"


class LampShow : public Scheduler {
  public:
    LampShow();
    ~LampShow();
    virtual void schedule();
    void playLampShow(const char filename[]);
  protected:
  private:
};

#endif // LAMPSHOW_H
