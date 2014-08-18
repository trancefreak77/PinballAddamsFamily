#ifndef COUNTERSPAN_H
#define COUNTERSPAN_H
#include <stdint.h>
uint32_t counterDiff(uint32_t startValue, uint32_t endValue);
uint32_t counterSpanToMs(uint32_t cntDeltaValue);   // Converts a counter value as time span to milliseconds.
uint32_t msToCounterSpan(uint32_t ms);              // Converts a time span of milliseconds to a cnt value.
#endif
