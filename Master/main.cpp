#include <propeller.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "Domain\SchedulerRegistry.h"
#include "Domain\AutonomousKicker.h"
#include "Game\PinballHSM.h"
#include "Lamps\LampShow.h"
#include "Domain\PlayfieldSwitchEnum.h"
#include "IODriverMailbox.h"

extern "C" void __cxa_pure_virtual() {
  while (1);
}

extern _Driver _SimpleSerialDriver;
// extern _Driver _FileDriver;

_Driver *_driverlist[] = {
  &_SimpleSerialDriver,
  // &_FileDriver,
  NULL
};

/*
 * This is the structure which we'll pass to the C cog.
 * It contains a small stack for working area, and the
 * mailbox which we use to communicate. See toggle.h
 * for the definition/
 */
HUBDATA static struct {
    uint32_t ioDriverStack[IO_DRIVER_STACK_SIZE];
    volatile ioDriverMailbox_t ioDriverMailbox;
} ioDriverPar;


// Forward declaration
void initializeScheduler();

// Playfield objects:
// ------------------
uint8_t inputPort[64];
uint8_t outputPort[64];

//PinballHSM game;
AutonomousKicker bumper1(outputPort[0], inputPort[0], 30);
AutonomousKicker bumper2(outputPort[1], inputPort[1], 30);
AutonomousKicker bumper3(outputPort[2], inputPort[2], 30);
AutonomousKicker bumper4(outputPort[3], inputPort[3], 30);
AutonomousKicker bumper5(outputPort[4], inputPort[4], 30);
//
AutonomousKicker slingshotLeft(outputPort[5], inputPort[5], 30);
AutonomousKicker slingshotRight(outputPort[6], inputPort[6], 30);

LampShow lampShow((uint8_t *)ioDriverPar.ioDriverMailbox.lampState, outputPort);

SchedulerRegistry schedulerRegistry;

/*
 * Function to start up a new cog running the IO driver
 * code (which we have placed in the IODriver.cog section)
 */
uint8_t startIODriver(volatile void *parptr) {
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

int main (void) {
  // Start the IO driver.
  uint8_t cogId = startIODriver(&ioDriverPar.ioDriverMailbox);

  // Initialize ports with test data.
  outputPort[0] = 250;
  outputPort[1] = 251;

  ioDriverPar.ioDriverMailbox.lampState[0] = 11;
  ioDriverPar.ioDriverMailbox.lampState[1] = 12;

  waitcnt(CLKFREQ + CNT);
//  printf("In main... Started ioDriver cogID: %d\n", cogId);
  //game.init();
  slingshotLeft.setNextActivationDeltaMs(40);
  slingshotRight.setNextActivationDeltaMs(40);

  //printf("After setting activation delta of slingshots...\n");
  initializeScheduler();

  //printf("After initializing scheduler...\n");
  lampShow.playLampShow(LampShow::Sequence::Multiball);

  uint32_t loopTicks[500];
  uint32_t loopCount = 0;

  PlayfieldSwitch pfSwitch = PlayfieldSwitch::ShooterLanePlungerSwitch;
  uint32_t x = (uint32_t) pfSwitch;

  while (1) {
    uint32_t startCnt = CNT;
    schedulerRegistry.schedule();
    uint32_t endCnt = CNT;

    loopTicks[loopCount] = endCnt - startCnt;
    loopCount++;

    if (loopCount == 500) {
      loopCount = 0;
      for (int z = 0; z < 500; z++) {
         // printf("main loop took ticks: %d\n", loopTicks[z]);
      }
    }

    // printf("IODriver loop took: %d\n", ioDriverPar.ioDriverMailbox.loopTicks);
  }
}

void initializeScheduler() {
  schedulerRegistry.addScheduler(bumper1, 5);
  schedulerRegistry.addScheduler(bumper2, 5);
  schedulerRegistry.addScheduler(bumper3, 5);
  schedulerRegistry.addScheduler(bumper4, 5);
  schedulerRegistry.addScheduler(bumper5, 5);
  schedulerRegistry.addScheduler(slingshotLeft, 5);
  schedulerRegistry.addScheduler(slingshotRight, 5);
  schedulerRegistry.addScheduler(lampShow, 33);
}
