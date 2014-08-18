#ifndef ONE_TIME_HIT_COUNTER_H
#define ONE_TIME_HIT_COUNTER_H
#include <stdint.h>

class OneTimeHitCounter {
  public:
    OneTimeHitCounter(uint8_t &inputPortValue);
    bool isImpule();

  private:
    bool _isActive;
    uint8_t &_inputPortValue;
};
#endif
