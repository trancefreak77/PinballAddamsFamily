#include "LampShow.h"
#include "MultiballLampShowWorker.h"
#include <stdio.h>
#include <propeller.h>
#include <stdint.h>

LampShow::LampShow(uint8_t lampState[], uint8_t outputPort[]) {
  _pLampState = lampState;
  _pOutputPort = outputPort;
  for (uint8_t i = 0; i < 3; i++) {
    _pWorkerArray[i] = nullptr;
  }
}

LampShow::~LampShow() {
  //dtor
}

void LampShow::finishedLampShow(LampShowWorker *lampShowWorker) {
//  printf("FINISHED LAMP SHOW!!!!\n");
  waitcnt(CNT + CLKFREQ);
  for (uint8_t i = 0; i < 3; i++) {
    if (_pWorkerArray[i] != nullptr && _pWorkerArray[i] == lampShowWorker) {
//      printf("Deleting worker index: %d\n", i);
      delete _pWorkerArray[i];
      _pWorkerArray[i] = nullptr;
    }
  }
}

void LampShow::schedule() {
  // printf("In LampShow::schedule...\n");
  Scheduler::schedule();
  for (uint8_t i = 0; i < 3; i++) {
    if (_pWorkerArray[i] != nullptr) {
      // printf("Calling lampShowWorker->schedule()...\n");
      _pWorkerArray[i]->schedule();
    }
  }
}
void LampShow::playLampShow(Sequence sequence) {
  // If all workers are full, we cannot
  // start a new one else start one.
//  printf("In playLampShow...\n");
//  printf("LampState[0] = %d\n", _lampState[0]);
//  printf("LampState[1] = %d\n", _lampState[1]);
//  waitcnt(CLKFREQ + CNT);
  int i = -1;
  for (uint8_t index = 0; index < 3; index++) {
    if (_pWorkerArray[index] == nullptr) {
      // We found an empty slot!
      i = index;
      //printf("Found an empty worker slot at index %d...\n", i);
      break;
    }
  }

  if (i < 0) {
    return;
  }

  switch (sequence) {
  case LampShow::Sequence::Multiball:
    //printf("Creating new multiball lampShowWorker...\n");
    _pWorkerArray[i] = new MultiballLampShowWorker(this, false);
    break;

  case LampShow::Sequence::BallPlunged:
    break;

  case LampShow::Sequence::Jackpot:
    break;

  case LampShow::Sequence::SuperJackpot:
    break;
  }
}

uint8_t *LampShow::getLampStateArray() {
  return _pLampState;
}

uint8_t *LampShow::getOutputPortArray() {
  return _pOutputPort;
}
