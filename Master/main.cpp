#include <propeller.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "Game\PinballHSM.h"
#include "Domain\SchedulerRegistry.h"
#include "Domain\PlayfieldSwitchEnum.h"
#include "CogC_Drivers\IODriver\IODriverMailbox.h"

#define MAX_LOOP_COUNTER 1
#define SCHEDULER_COG_STACKSIZE 500

extern "C" void __cxa_pure_virtual() {
  while (1);
}

extern _Driver _SimpleSerialDriver;
//extern _Driver _FullDuplexSerialDriver;
extern _Driver _FileDriver;

/* Driver list */
_Driver *_driverlist[] = {
    &_SimpleSerialDriver,
    &_FileDriver,
    NULL
};

static struct {
  uint32_t ioDriverStack[IO_DRIVER_STACK_SIZE];
  volatile ioDriverMailbox_t ioDriverMailbox;
} ioDriverPar;

// Scheduler cog stack declaration
static uint8_t schedulerCogStack[EXTRA_STACK_BYTES + SCHEDULER_COG_STACKSIZE];      // allocate space for the stack - you need to set up a
                                                                                    // stack for each Cog you plan on starting

const uint32_t testIndex = 5000;
volatile uint8_t testArr[testIndex];
const uint8_t activated = 128;
PinballHSM game((ioDriverMailbox_t&) ioDriverPar.ioDriverMailbox);

/*
 * Function to start up a new cog running the IO driver
 * code (which we have placed in the IODriver.cog section)
 */
uint8_t startIODriver(volatile void *parptr) {
  extern int _C_LOCK;
  _C_LOCK |= 0x100;

  extern unsigned int _load_start_IODriver_cog[];
  #if defined(__PROPELLER_XMM__) || defined(__PROPELLER_XMMC__)
    uint32_t hub_buffer[496];
    extern unsigned int _load_stop_IODriver_cog[];
    memcpy(hub_buffer, _load_start_IODriver_cog, (_load_stop_IODriver_cog - _load_start_IODriver_cog) * sizeof(uint32_t));
    return cognew(hub_buffer, parptr);
  #else
    return cognew(_load_start_IODriver_cog, parptr);
  #endif
}

void schedulerCog(void* par) {
  // Schedule tasks.
  SchedulerRegistry* pSchedulerRegistry = (SchedulerRegistry*) par;
  while(1) {
    pSchedulerRegistry->schedule();
  }
}

void fillPattern(uint8_t* pFillArray, uint32_t count) {
  for (uint32_t index = 0; index < (count - 1); index++) {
    *(pFillArray + index) = 0xAA;

    if (index + 1 < count) {
      *(pFillArray + index + 1) = 0xBB;
    }

    index++;
  }
}

int findPattern(uint8_t* checkArray, uint32_t count) {
  for (uint32_t index = 0; index < count - 1; index++) {
    if (*(checkArray + index) == 0xAA && *(checkArray + index + 1) == 0xBB) {
      return index;
    }

    index++;
  }

  return -1;
}

void handleSwitches(uint8_t);

int main (void) {
  game.init();
  SchedulerRegistry& schedulerRegistry = game.getSchedulerRegistry();
//  fillPattern((uint8_t*) ioDriverPar.ioDriverStack, IO_DRIVER_STACK_SIZE * 4);
  fillPattern((uint8_t*) schedulerCogStack, EXTRA_STACK_BYTES + SCHEDULER_COG_STACKSIZE);
//  schedulerRegistry.addScheduler(lampShow, 31);
//  lampShow.playLampShow(LampShow::Sequence::BallPlunged);

  // Start the IO driver cog.
  startIODriver(&ioDriverPar.ioDriverMailbox);
  cogstart(schedulerCog, &schedulerRegistry, schedulerCogStack, sizeof(schedulerCogStack));

//  waitcnt((80000000 * 5) + CNT);
//  int foundPos = findPattern((uint8_t*) ioDriverPar.ioDriverStack, IO_DRIVER_STACK_SIZE * 4);
//  printf("Found first pattern of ioDriverStack at %d\n", foundPos);
//  foundPos = findPattern((uint8_t*) schedulerCogStack, EXTRA_STACK_BYTES + 2000);
//  printf("Found first pattern of schedulerCogStack at %d\n", foundPos);

  // Global variables / objects
  uint8_t switchKey = 0;
  ioDriverPar.ioDriverMailbox.lampState[0] = 2;

  for (uint32_t y = 0; y < testIndex; y++) {
    testArr[y] = 1;
  }

  uint8_t previousSwitchInpurtPort[64];
  for (uint8_t loopCounter = 0; loopCounter < 64; loopCounter++) {
    // Initialize with value 2 which means, not closed and not open.
    previousSwitchInpurtPort[loopCounter] = 2;
  }

  waitcnt((80000000 * 5) + CNT);
  int foundPos = findPattern((uint8_t*) schedulerCogStack, EXTRA_STACK_BYTES + SCHEDULER_COG_STACKSIZE);
  printf("Found first pattern of schedulerCogStack at %d\n", foundPos);

  while (1) {
//    printf("IODriver loop took: %d\n", ioDriverPar.ioDriverMailbox.loopTicks);
    // Handle only switch change events.
    for (uint8_t i = 0; i < 64; i++) {
      if (previousSwitchInpurtPort[i] == ioDriverPar.ioDriverMailbox.inputPort[i]) {
        // No change happened.
        continue;
      }

      // Switch state changed, notify game state machine.
      previousSwitchInpurtPort[i] = ioDriverPar.ioDriverMailbox.inputPort[i];
      switchKey = i;

      if (ioDriverPar.ioDriverMailbox.inputPort[i] > 0) {   // Switch is activated.
        switchKey = i | activated;
      }

      // Now call the appropriate switch function.
      handleSwitches(switchKey);
    }

//    waitcnt(20000000 + CNT);
  }
}

void handleSwitches(uint8_t switchKey) {
  switch (switchKey) {
    case (int)PlayfieldSwitch::ShooterLanePlungerSwitch :
      game.onShooterLanePlungerSwitchClosed();
      break;

    case (int)PlayfieldSwitch::ShooterLanePlungerSwitch | activated :
      game.onShooterLanePlungerSwitchOpened();
      break;

    case (int)PlayfieldSwitch::ShooterLaneRampSwitch :
      game.onShooterLaneRampSwitchClosed();
      break;

    case (int)PlayfieldSwitch::ShooterLaneRampSwitch | activated :
      game.onShooterLaneRampSwitchOpened();
      break;

    case (int)PlayfieldSwitch::OuterLoopRightSwitch :
      game.onOuterLoopRightSwitchClosed();
      break;

    case (int)PlayfieldSwitch::OuterLoopRightSwitch | activated :
      game.onOuterLoopRightSwitchOpened();
      break;

    case (int)PlayfieldSwitch::OuterLoopLeftSwitch :
      game.onOuterLoopLeftSwitchClosed();
      break;

    case (int)PlayfieldSwitch::OuterLoopLeftSwitch | activated :
      game.onOuterLoopLeftSwitchOpened();
      break;

    case (int)PlayfieldSwitch::ThingRampSwitch :
      game.onThingRampSwitchClosed();
      break;

    case (int)PlayfieldSwitch::ThingRampSwitch | activated :
      game.onThingRampSwitchOpened();
      break;

    case (int)PlayfieldSwitch::ThingHoleSwitch :
      game.onThingHoleSwitchClosed();
      break;

    case (int)PlayfieldSwitch::ThingHoleSwitch | activated :
      game.onThingHoleSwitchOpened();
      break;

    case (int)PlayfieldSwitch::FarLeftInlaneSwitch :
      game.onFarLeftInlaneSwitchClosed();
      break;

    case (int)PlayfieldSwitch::FarLeftInlaneSwitch | activated :
      game.onFarLeftInlaneSwitchOpened();
      break;

    case (int)PlayfieldSwitch::LeftOutlaneSwitch :
      game.onLeftOutlaneSwitchClosed();
      break;

    case (int)PlayfieldSwitch::LeftOutlaneSwitch | activated :
      game.onLeftOutlaneSwitchOpened();
      break;

    case (int)PlayfieldSwitch::InnerLeftInlaneSwitch :
      game.onLeftInnerInlaneSwitchClosed();
      break;

    case (int)PlayfieldSwitch::InnerLeftInlaneSwitch | activated :
      game.onLeftInnerInlaneSwitchOpened();
      break;

    case (int)PlayfieldSwitch::RightInlaneSwitch :
      game.onRightInlaneSwitchClosed();
      break;

    case (int)PlayfieldSwitch::RightInlaneSwitch | activated :
      game.onRightInlaneSwitchOpened();
      break;

    case (int)PlayfieldSwitch::RightOutlaneSwitch :
      game.onRightOutlaneSwitchClosed();
      break;

    case (int)PlayfieldSwitch::RightOutlaneSwitch | activated :
      game.onRightOutlaneSwitchOpened();
      break;

    case (int)PlayfieldSwitch::Bumper1Switch :
      game.onBumper1SwitchOpened();
      break;

    case (int)PlayfieldSwitch::Bumper1Switch | activated :
      game.onBumper1SwitchOpened();
      break;

    case (int)PlayfieldSwitch::Bumper2Switch :
      game.onBumper2SwitchClosed();
      break;

    case (int)PlayfieldSwitch::Bumper2Switch | activated :
      game.onBumper2SwitchOpened();
      break;

    case (int)PlayfieldSwitch::Bumper3Switch :
      game.onBumper3SwitchClosed();
      break;

    case (int)PlayfieldSwitch::Bumper3Switch | activated :
      game.onBumper3SwitchOpened();
      break;

    case (int)PlayfieldSwitch::Bumper4Switch :
      game.onBumper4SwitchClosed();
      break;

    case (int)PlayfieldSwitch::Bumper4Switch | activated :
      game.onBumper4SwitchOpened();
      break;

    case (int)PlayfieldSwitch::Bumper5Switch :
      game.onBumper5SwitchClosed();
      break;

    case (int)PlayfieldSwitch::Bumper5Switch | activated :
      game.onBumper5SwitchOpened();
      break;

    case (int)PlayfieldSwitch::LeftFlipperButtonSwitch :
      game.onLeftFlipperButtonSwitchClosed();
      break;

    case (int)PlayfieldSwitch::LeftFlipperButtonSwitch | activated :
      game.onLeftFlipperButtonSwitchOpened();
      break;

    case (int)PlayfieldSwitch::RightFlipperButtonSwitch :
      game.onRightFlipperButtonSwitchClosed();
      break;

    case (int)PlayfieldSwitch::RightFlipperButtonSwitch | activated :
      game.onRightFlipperButtonSwitchOpened();
      break;
  }
}
