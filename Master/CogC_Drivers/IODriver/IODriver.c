#include <stdint.h>
#include <stdbool.h>
#include <propeller.h>
#include "IODriverMailbox.h"

#define OUTPUT 1
#define INPUT 0
#define LOW 0
#define HIGH 1

#define	SHIFT_OUT_DATA_PIN				11
#define SHIFT_OUT_LATCH_PIN				12
#define CLOCK_PIN						      17
#define MAX_7221_DATA_PIN				  15
#define MAX_7221_CHIP_SELECT_PIN	16
#define SHIFT_IN_DATA_PIN				  13
#define SHIFT_IN_LATCH_PIN				14

#define SHIFT_OUT_DATA_PIN_MASK		    (1u << SHIFT_OUT_DATA_PIN)
#define SHIFT_OUT_LATCH_PIN_MASK      (1u << SHIFT_OUT_LATCH_PIN)
#define MAX_7221_DATA_PIN_MASK			  (1u << MAX_7221_DATA_PIN)
#define MAX_7221_CHIP_SELECT_PIN_MASK	(1u << MAX_7221_CHIP_SELECT_PIN)
#define SHIFT_IN_DATA_PIN_MASK			  (1u << SHIFT_IN_DATA_PIN)
#define SHIFT_IN_LATCH_PIN_MASK			  (1u << SHIFT_IN_LATCH_PIN)
#define CLOCK_PIN_MASK					      (1u << CLOCK_PIN)

#define WAIT_TICKS_ONE_MS 80000               //    1 ms
#define KICKER_ACTIVATION_TICKS (WAIT_TICKS_ONE_MS * 20)
#define KICKER_DEAD_TICKS (WAIT_TICKS_ONE_MS * 40)

static _COGMEM unsigned int shiftedInValue;
static _COGMEM unsigned int value;
static _COGMEM unsigned int mask;
static _COGMEM unsigned int bits;
static _COGMEM unsigned int i;
static _COGMEM unsigned int looper;
static _COGMEM unsigned int outputPortA;
static _COGMEM unsigned int outputPortB;
static _COGMEM unsigned int startCnt;
static _COGMEM unsigned int endCnt;
static _COGMEM uint32_t shiftOutEndMask;
static _COGMEM uint8_t byteValue;
static _COGMEM uint8_t _lightMatrixColumn;
static _COGMEM uint8_t _lightMatrixRow;
static _COGMEM uint8_t _inputPortNumber;
static _COGMEM uint8_t _outputPortNumber;
static const _COGMEM uint32_t MAX_COUNTER_VALUE = 4294967295u;


/*
 * Returns the delta ticks between a given
 * start and end cnt value.
 * param startValue = start CNT value
 * param endValue = end CNT value
 */
_NATIVE
uint32_t counterDiff(uint32_t startValue, uint32_t endValue) {
  if (endValue > startValue) {
    return (endValue - startValue);
  }

  return (MAX_COUNTER_VALUE - startValue + endValue);
}

/*
 * Set a pin high without affecting other pins.
 * param WCpin = pin number to set high.
 */
_NATIVE
void setPinHigh(unsigned int WCpin) {
    bits = 1 << WCpin;
    DIRA |= bits;
    OUTA |= bits;
}

/*
 * Set a pin low without affecting other pins.
 * param WCpin = pin number to set low.
 */
_NATIVE
void setPinLow(unsigned int WCpin) {
    mask = 1 << WCpin;
    DIRA |= mask;
    OUTA &= ~mask;
}

// Sets the pin to output or input.
_NATIVE
void setPinMode(unsigned int pinNumber, unsigned int direction) {
    mask = 1u << pinNumber;

    if (direction == INPUT) {
        DIRA &= ~mask;
    } else {
        DIRA |= mask;
    }
}

_NATIVE
void shiftOutInt(unsigned int valueToShiftOut, uint32_t dataPinMask, uint32_t numberBitsToSend) {
  if (numberBitsToSend == 32) {
    shiftOutEndMask = 0;
  } else {
    shiftOutEndMask = 1u << (31u - numberBitsToSend);
  }
  // Set clock pin to low.
  OUTA &= ~CLOCK_PIN_MASK;

  // Shift out 32 bits with MSB first.
  for (mask = 0x80000000; mask > shiftOutEndMask; mask >>= 1) {
    if ((valueToShiftOut & mask) > 0) {
      // Set data pin to 1.
      OUTA |= dataPinMask;
    } else {
      // Set data pin to 0.
      OUTA &= ~dataPinMask;
    }

    //waitcnt(10 + CNT);

    // Set clock pin high
    OUTA |= CLOCK_PIN_MASK;

    //waitcnt(10 + CNT);

    // Set clock pin low
    OUTA &= ~CLOCK_PIN_MASK;
  }
}

_NATIVE
void shiftInInt() {
  shiftedInValue = 0;
  // Set clock pin to low.
  OUTA &= ~CLOCK_PIN_MASK;

  for (i = 0; i < 32; i++) {
    // Read data pin.
    value = INA & SHIFT_IN_DATA_PIN_MASK;

    if (value > 0) {
      // Set most right bit to 1
      shiftedInValue |= 1 << (31 - i);
    }

    // shiftedInValue <<= 1;

    // Set clock high. This forces chip to
    // write the value to the data pin.
    OUTA |= CLOCK_PIN_MASK;
    OUTA &= ~CLOCK_PIN_MASK;
  }
}

_NATIVE
void sendMax7221Command(uint32_t command, uint32_t data) {
  command = ((command << 8) | data) << 16;
  OUTA &= ~MAX_7221_CHIP_SELECT_PIN_MASK;
  // waitcnt(80 + CNT);
  shiftOutInt(command, MAX_7221_DATA_PIN_MASK, 16);
  OUTA |= MAX_7221_CHIP_SELECT_PIN_MASK;
  // waitcnt(80 + CNT);
}

_NATIVE
void initMax7221() {
  // Init Max7221:
  // Set chip select to high
  OUTA |= MAX_7221_CHIP_SELECT_PIN_MASK;

  // Set clock pin low
  OUTA &= ~CLOCK_PIN_MASK;

  // Wait 50 microseconds.
  waitcnt(80 + CNT);

  // Set no decode mode.
  sendMax7221Command(9, 0);
  // Test mode off.
  sendMax7221Command(0x0F, 0);
  // Set display scan limit.
  sendMax7221Command(0x0B, 7);
  // Set normal mode.
  sendMax7221Command(0x0C, 1);
  // Set intensity to 15 (max 15).
  sendMax7221Command(0x0A, 15);

  // Light off all rows.
  for (i = 1; i < 9; i++) {
    sendMax7221Command(i, 0);
  }
}

_NATIVE
void updateKickerObjects(ioDriverMailbox_t *m) {
  for (i = 0; i < 10; i++) {
    _inputPortNumber = m->kickerToInputPortMapping[i];
    _outputPortNumber = m->kickerToOutputPortMapping[i];

    if (m->inputPort[_inputPortNumber] > 0 && !(m->isKickerCoilActive[i])) {
      // Bumper is not active but it should be.
      // Check if bumper can be activated and the
      // register dead time is elapsed.
      if (counterDiff(m->kickerActivationCnt[i], CNT) < KICKER_DEAD_TICKS) {
        // Kicker must not be enabled again.
        return;
      }

      // Activate coil output port.
      m->isKickerCoilActive[i] = true;
      m->kickerActivationCnt[i] = CNT;
      m->outputPort[_outputPortNumber] = 1;
      return;
    }

    // If bumper is active but activation time is
    // elapsed, deactivate it.
    if (m->isKickerCoilActive[i] && counterDiff(m->kickerActivationCnt[i], CNT) > KICKER_ACTIVATION_TICKS) {
      m->isKickerCoilActive[i] = false;
      m->outputPort[_outputPortNumber] = 0;
      return;
    }
  }
}

_NAKED
int main(ioDriverMailbox_t *m) {
  // Setup all pin modes
  setPinMode(11, OUTPUT);
  setPinMode(12, OUTPUT);
  setPinMode(17, OUTPUT);

  // Setup max7221 pin modes
  setPinMode(15, OUTPUT);
  setPinMode(16, OUTPUT);

  // Setup shift in pin modes
  setPinMode(13, INPUT);
  setPinMode(14, OUTPUT);

  initMax7221();

  // Main loop
  while (1)   {
    startCnt = CNT;
    outputPortA = 0;
    outputPortB = 0;

    // Output port A
    for (i = 0; i < 32; i++) {
      if (m->outputPort[i] > 0) {
        outputPortA |= (1 << i);
      }
    }

    // Output port B
    for (i = 32; i < 64; i++) {
      if (m->outputPort[i] > 0) {
        outputPortB |= (1 << (i - 32));
      }
    }

    // Shift OUT
    // Set latch and clock pin low
    setPinLow(CLOCK_PIN);
    setPinLow(SHIFT_OUT_LATCH_PIN);

    // Shift out values
    shiftOutInt(outputPortB, SHIFT_OUT_DATA_PIN_MASK, 32);
    shiftOutInt(outputPortA, SHIFT_OUT_DATA_PIN_MASK, 32);

    // Set latch pin high to store
    // the values.
    setPinHigh(SHIFT_OUT_LATCH_PIN);

    // Shift IN
    setPinLow(SHIFT_IN_LATCH_PIN);
    setPinHigh(SHIFT_IN_LATCH_PIN);
    shiftInInt();

    mask = 1u;
    for (i = 32; i < 64; i++) {
      if ((shiftedInValue & mask) > 0) {
        m->inputPort[i] = 1;
      } else {
        m->inputPort[i] = 0;
      }

      mask <<= 1;
    }

    shiftInInt();

    mask = 1u;
    for (i = 0; i < 32; i++) {
      if ((shiftedInValue & mask) > 0) {
        m->inputPort[i] = 1;
      } else {
        m->inputPort[i] = 0;
      }

      mask <<= 1;
    }

    // Handle Max7221: Create light column/rows
    byteValue = 0;
    _lightMatrixRow = 1;    // Initialize for row 1.
    _lightMatrixColumn = 0; // Initialize for column 1.
    for (i = 0; i < 65; i++) {
      if (m->lampState[i] > 0) {
        byteValue |= 1u << _lightMatrixColumn;
      }

      if (_lightMatrixColumn == 7) {
        // Row complete.
        value = ((_lightMatrixRow << 8) | byteValue) << 16;
        OUTA &= ~MAX_7221_CHIP_SELECT_PIN_MASK;
        //waitcnt(10 + CNT);
        shiftOutInt(value, MAX_7221_DATA_PIN_MASK, 16);
        OUTA |= MAX_7221_CHIP_SELECT_PIN_MASK;
        //waitcnt(10 + CNT);
        _lightMatrixColumn = 0;
        _lightMatrixRow++;
      }

      _lightMatrixColumn++;
    }

    updateKickerObjects(m);
    endCnt = CNT;
    m->loopTicks = endCnt - startCnt;
  }
  return 0;
}
