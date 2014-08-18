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
extern _Driver _FileDriver;

_Driver *_driverlist[] = {
  &_SimpleSerialDriver,
  &_FileDriver,
  NULL
};

// Playfield objects:
// ------------------
uint8_t inputPort[64];
uint8_t outputPort[64];

//PinballHSM game;
//AutonomousKicker bumper1(outputPort[0], inputPort[0], 30);
//AutonomousKicker bumper2(outputPort[1], inputPort[1], 30);
//AutonomousKicker bumper3(outputPort[2], inputPort[2], 30);
//AutonomousKicker bumper4(outputPort[3], inputPort[3], 30);
//AutonomousKicker bumper5(outputPort[4], inputPort[4], 30);
//
//AutonomousKicker slingshotLeft(outputPort[5], inputPort[5], 30);
//AutonomousKicker slingshotRight(outputPort[6], inputPort[6], 30);

LampShow lampShow;

SchedulerRegistry schedulerRegistry;

int main (void) {
  //game.init();
  //slingshotLeft.setNextActivationDeltaMs(40);
  //slingshotRight.setNextActivationDeltaMs(40);

  waitcnt(CLKFREQ + CNT);
  printf("Starting...\n");
  lampShow.playLampShow("mb.txt");
  while (1) {
  }
}

void initializeScheduler() {
//  schedulerRegistry.addScheduler(bumper1, 5);
//  schedulerRegistry.addScheduler(bumper2, 5);
//  schedulerRegistry.addScheduler(bumper3, 5);
//  schedulerRegistry.addScheduler(bumper4, 5);
//  schedulerRegistry.addScheduler(bumper5, 5);
//  schedulerRegistry.addScheduler(slingshotLeft, 5);
//  schedulerRegistry.addScheduler(slingshotRight, 5);
}
