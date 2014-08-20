#ifndef OUTPUT_ACTIVATOR_H
#define OUTPUT_ACTIVATOR_H
#include <stdint.h>
#include <stdbool.h>

class OutputActivator {
  public:
    OutputActivator(uint8_t &outputPortValue, uint32_t activationTimeMs);
    void activate();
    void update();
    void setActivationTimeMs(uint32_t activationTimeMs);
    uint32_t getActivationTimeMs();

  private:
    bool _isActive;
    uint8_t &_outputPortValue;                        // Pointer to the current state of the output port.
    uint32_t _startCounterValue;                      // Counter value when the output was set to enabled.
    uint32_t _activeCounterTicks;                     // How many system counter ticks should the output be enabled.
    uint32_t _activationTimeMs;						            // How many milliseconds should the output be activated.
};
#endif
