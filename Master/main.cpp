#include <propeller.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <map>
#include "Domain\SchedulerRegistry.h"
#include "Domain\AutonomousKicker.h"
#include "Game\PinballHSM.h"
#include "Lamps\LampShow.h"
#include "Domain\PlayfieldSwitchEnum.h"
#include "CogC_Drivers\IODriver\IODriverMailbox.h"
#include "libpropeller\sd\sd.h"

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


// Forward declaration.
void initializeScheduler();
void initializeEventNotificationMap();

// Define member function pointer map.
typedef void (PinballHSM::*MFP)();
std::map<uint8_t, MFP> fMap;

PinballHSM game;

// Playfield objects:
// The objects directly react on the state of the assigned input port and
// use the configured output port. Counting points will not be done by these
// playfield objects. This is done in the main game loop by using the
// function map.
//AutonomousKicker bumper1((uint8_t &)ioDriverPar.ioDriverMailbox.outputPort[0], (uint8_t &)ioDriverPar.ioDriverMailbox.inputPort[0], 30);
//AutonomousKicker bumper2((uint8_t &)ioDriverPar.ioDriverMailbox.outputPort[1], (uint8_t &)ioDriverPar.ioDriverMailbox.inputPort[1], 30);
//AutonomousKicker bumper3((uint8_t &)ioDriverPar.ioDriverMailbox.outputPort[2], (uint8_t &)ioDriverPar.ioDriverMailbox.inputPort[2], 30);
//AutonomousKicker bumper4((uint8_t &)ioDriverPar.ioDriverMailbox.outputPort[3], (uint8_t &)ioDriverPar.ioDriverMailbox.inputPort[3], 30);
//AutonomousKicker bumper5((uint8_t &)ioDriverPar.ioDriverMailbox.outputPort[4], (uint8_t &)ioDriverPar.ioDriverMailbox.inputPort[4], 30);
////
//AutonomousKicker slingshotLeft((uint8_t &)ioDriverPar.ioDriverMailbox.outputPort[5], (uint8_t &)ioDriverPar.ioDriverMailbox.inputPort[5], 30);
//AutonomousKicker slingshotRight((uint8_t &)ioDriverPar.ioDriverMailbox.outputPort[6], (uint8_t &)ioDriverPar.ioDriverMailbox.inputPort[6], 30);

LampShow lampShow((uint8_t *)ioDriverPar.ioDriverMailbox.lampState, (uint8_t *)ioDriverPar.ioDriverMailbox.outputPort);

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
  // Start the IO driver cog.
  startIODriver(&ioDriverPar.ioDriverMailbox);

  // Initialize ports with test data.
  waitcnt(CLKFREQ + CNT);

  game.init();
//  slingshotLeft.setNextActivationDeltaMs(40);
//  slingshotRight.setNextActivationDeltaMs(40);

  initializeEventNotificationMap();
  initializeScheduler();

  uint8_t previousSwitchInpurtPort[64];
  for (int i = 0; i < 64; i++) {
    // Initialize with value 2 which means, not closed and not open.
    previousSwitchInpurtPort[i] = 2;
  }

  int startCnt = CNT;
  int endCnt = CNT;
  while (1) {
    startCnt = CNT;
    // Schedule tasks.
    schedulerRegistry.schedule();

    // Handle only switch change events.
    for (int i = 0; i < 64; i++) {
      if (previousSwitchInpurtPort[i] == ioDriverPar.ioDriverMailbox.inputPort[i]) {
        // No change happened.
        continue;
      }

      // Switch state changed, notify game state machine.
      previousSwitchInpurtPort[i] = ioDriverPar.ioDriverMailbox.inputPort[i];

      // Currently not used.
      // PlayfieldSwitch playfieldSwitch = (PlayfieldSwitch) i;

      uint8_t functionMapLookupKey = i;
      if (ioDriverPar.ioDriverMailbox.inputPort[i] == 0) {
        functionMapLookupKey |= 0;      // Means switch is open (inactive).
      } else {
        functionMapLookupKey |= 128;    // Means switch is closed (active).
      }

      // Enable the following two lines if all
      // 64 destination entries are done into fMap!
//      MFP fp = fMap[functionMapLookupKey];
//      (game.*fp)();
    }

    endCnt = CNT;
//    printf("Loop took %d ticks...\n", endCnt - startCnt);
//    waitcnt(20000000 + CNT);
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
  schedulerRegistry.addScheduler(lampShow, 33);
}

void initializeEventNotificationMap() {
  // | 0   == Switch open and inactive
  // | 128 == Switch closed and active
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::ShooterLanePlungerSwitch)  | 0,   &PinballHSM::onShooterLanePlungerSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::ShooterLanePlungerSwitch)  | 128, &PinballHSM::onShooterLanePlungerSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::ShooterLaneRampSwitch)     | 0,   &PinballHSM::onShooterLaneRampSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::ShooterLaneRampSwitch)     | 128, &PinballHSM::onShooterLaneRampSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::OuterLoopRightSwitch)      | 0,   &PinballHSM::onOuterLoopRightSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::OuterLoopRightSwitch)      | 128, &PinballHSM::onOuterLoopRightSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::OuterLoopLeftSwitch)       | 0,   &PinballHSM::onShooterLanePlungerSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::OuterLoopLeftSwitch)       | 128, &PinballHSM::onShooterLanePlungerSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::ThingRampSwitch)           | 0,   &PinballHSM::onShooterLaneRampSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::ThingRampSwitch)           | 128, &PinballHSM::onShooterLaneRampSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::ThingHoleSwitch)           | 0,   &PinballHSM::onOuterLoopRightSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::ThingHoleSwitch)           | 128, &PinballHSM::onOuterLoopRightSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::FarLeftInlaneSwitch)       | 0,   &PinballHSM::onFarLeftInlaneSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::FarLeftInlaneSwitch)       | 128, &PinballHSM::onFarLeftInlaneSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::LeftOutlaneSwitch)         | 0,   &PinballHSM::onLeftOutlaneSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::LeftOutlaneSwitch)         | 128, &PinballHSM::onLeftOutlaneSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::InnerLeftInlaneSwitch)     | 0,   &PinballHSM::onLeftInnerInlaneSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::InnerLeftInlaneSwitch)     | 128, &PinballHSM::onLeftInnerInlaneSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::RightInlaneSwitch)         | 0,   &PinballHSM::onRightInlaneSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::RightInlaneSwitch)         | 128, &PinballHSM::onRightInlaneSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::RightOutlaneSwitch)        | 0,   &PinballHSM::onRightOutlaneSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::RightOutlaneSwitch)        | 128, &PinballHSM::onRightOutlaneSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::Bumper1Switch)             | 0,   &PinballHSM::onBumper1SwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::Bumper1Switch)             | 128, &PinballHSM::onBumper1SwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::Bumper2Switch)             | 0,   &PinballHSM::onBumper2SwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::Bumper2Switch)             | 128, &PinballHSM::onBumper2SwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::Bumper3Switch)             | 0,   &PinballHSM::onBumper3SwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::Bumper3Switch)             | 128, &PinballHSM::onBumper3SwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::Bumper4Switch)             | 0,   &PinballHSM::onBumper4SwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::Bumper4Switch)             | 128, &PinballHSM::onBumper4SwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::Bumper5Switch)             | 0,   &PinballHSM::onBumper5SwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::Bumper5Switch)             | 128, &PinballHSM::onBumper5SwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::LeftFlipperButtonSwitch)   | 0,   &PinballHSM::onLeftFlipperButtonSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::LeftFlipperButtonSwitch)   | 128, &PinballHSM::onLeftFlipperButtonSwitchClosed));

  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::LeftFlipperButtonSwitch)   | 0,   &PinballHSM::onLeftFlipperButtonSwitchOpened));
  fMap.insert(std::make_pair(((uint8_t) PlayfieldSwitch::LeftFlipperButtonSwitch)   | 128, &PinballHSM::onLeftFlipperButtonSwitchClosed));
}

void playLampShow() {
  lampShow.playLampShow(LampShow::Sequence::Multiball);
}
