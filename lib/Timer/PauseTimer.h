#ifndef PAUSETIMER_H
#define PAUSETIMER_H
#include <stdint.h>

class PauseTimer {
  public:
    PauseTimer();
    ~PauseTimer();
    void start();
    bool isElapsed(uint16_t intervalMs);
  protected:
  private:
    uint16_t _intervalMs;
    uint32_t _startCnt;
};

#endif // PAUSETIMER_H
