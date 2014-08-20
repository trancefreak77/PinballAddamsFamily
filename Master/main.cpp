#include <propeller.h>
#include <stdlib.h>
#include <stdio.h>
#include "Domain\SchedulerRegistry.h"
#include "Domain\AutonomousKicker.h"
#include "Game\PinballHSM.h"
#include "Lamps\LampShow.h"

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

LampShow lampShow;

SchedulerRegistry schedulerRegistry;

int main (void) {
  waitcnt(CLKFREQ + CNT);
  // printf("In main...\n");
  //game.init();
  slingshotLeft.setNextActivationDeltaMs(40);
  slingshotRight.setNextActivationDeltaMs(40);

  //printf("After setting activation delta of slingshots...\n");
  initializeScheduler();

  //printf("After initializing scheduler...\n");
  lampShow.playLampShow(LampShow::Sequence::Multiball);

  int loopTicks[500];
  int loopCount = 0;

  while (1) {
    int startCnt = CNT;
    schedulerRegistry.schedule();
    int endCnt = CNT;

    loopTicks[loopCount] = endCnt - startCnt;
    loopCount++;

    if (loopCount == 500) {
      loopCount = 0;
      //for (int z = 0; z < 500; z++) {
        // printf("main loop took ticks: %d\n", loopTicks[z]);
      //}
    }
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
