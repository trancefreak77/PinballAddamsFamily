#include <stdint.h>
#include <propeller.h>
#include "CounterSpan.h"

const uint32_t MAX_COUNTER_VALUE = 4294967295u;

uint32_t counterDiff(uint32_t startValue, uint32_t endValue) {
  if (endValue > startValue) {
    return (endValue - startValue);
  }

  return (MAX_COUNTER_VALUE - startValue + endValue);
}

uint32_t counterSpanToMs(uint32_t cntDeltaValue) {
  return cntDeltaValue / (CLKFREQ / 1000);
}

uint32_t msToCounterSpan(uint32_t ms) {
  return (CLKFREQ / 1000) * ms;
}
