#include <propeller.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "Game\PinballHSM.h"
#include "Domain\SchedulerRegistry.h"
#include "Lamps\LampShow.h"
#include "Domain\PlayfieldSwitchEnum.h"
#include "CogC_Drivers\IODriver\IODriverMailbox.h"
#include "libpropeller\serial\serial.h"
#include "libpropeller\sd\sd.h"

extern "C" void __cxa_pure_virtual() {
  while (1);
}

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

// Global variables / objects
uint8_t switchKey = 0;
const uint8_t activated = 128;
LampShow lampShow((uint8_t*) ioDriverPar.ioDriverMailbox.lampState, (uint8_t*) ioDriverPar.ioDriverMailbox.outputPort);
SchedulerRegistry schedulerRegistry;
PinballHSM game;

// Define and create serial object
const int rxpin = 31;
const int txpin = 30;
const int baud = 115200; // 460800;
libpropeller::Serial serial;

// Define and create SD card object
const int kDoPin = 10;
const int kClkPin = 9;
const int kDiPin = 8;
const int kCsPin = 7;
// libpropeller::SD sdObj;

// Forward declaration.
void initializeScheduler();
void initializeEventNotificationMap();
void handleSwitches(uint8_t);
void playLampShow();

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
  // Mount SD card
  // FOR TESTING ONLY! USE SD CARD
//  sdObj.ClearError();
//  sdObj.Mount(kDoPin, kClkPin, kDiPin, kCsPin);

  // Start the IO driver cog.
  waitcnt((CLKFREQ / 4) + CNT);
  // Initialize ports with test data.
  serial.Start(rxpin, txpin, baud);
  waitcnt((CLKFREQ / 4) + CNT);

  serial.PutFormatted("This is COGID: %d\n", cogid());
  serial.PutFormatted("Starting DriverIO COGID: %d\n", startIODriver(&ioDriverPar.ioDriverMailbox));

  waitcnt((CLKFREQ / 4) + CNT);

  game.init();
  initializeScheduler();

  uint8_t previousSwitchInpurtPort[64];
  for (uint8_t loopCounter = 0; loopCounter < 64; loopCounter++) {
    // Initialize with value 2 which means, not closed and not open.
    previousSwitchInpurtPort[loopCounter] = 1;
  }

  int startCnt = CNT;
  int endCnt = CNT;
  playLampShow();

  while (1) {
    // serial.Put("Test!\n");
    startCnt = CNT;
    // Schedule tasks.
    schedulerRegistry.schedule();

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

    endCnt = CNT;
    // printf("Loop took: %d\n", (endCnt - startCnt));
    serial.PutFormatted("Loop took: %d\n", (endCnt - startCnt));
    waitcnt((CLKFREQ / 4) + CNT);
  }
}

void initializeScheduler() {
  schedulerRegistry.addScheduler(lampShow, 1000);
}

void playLampShow() {
  lampShow.playLampShow(LampShow::Sequence::Multiball);
}

void handleSwitches(uint8_t switchKey) {
  // printf("In handle switches...\n");
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
