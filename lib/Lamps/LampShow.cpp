#include "LampShow.h"
#include "MultiballLampShowWorker.h"
#include <stdio.h>
#include <propeller.h>

LampShow::LampShow() {
  for (int i = 0; i < 3; i++) {
    _pWorkerArray[i] = nullptr;
  }
}

LampShow::~LampShow() {
  //dtor
}

void LampShow::finishedLampShow(LampShowWorker *lampShowWorker) {
  // printf("FINISHED LAMP SHOW!!!!\n");
  for (int i = 0; i < 3; i++) {
    if (_pWorkerArray[i] != nullptr && _pWorkerArray[i] == lampShowWorker) {
      delete _pWorkerArray[i];
      _pWorkerArray[i] = nullptr;
    }
  }
}

void LampShow::schedule() {
  // printf("In LampShow::schedule...\n");
  Scheduler::schedule();
  for (int i = 0; i < 3; i++) {
    if (_pWorkerArray[i] != nullptr) {
      // printf("Calling lampShowWorker->schedule()...\n");
      _pWorkerArray[i]->schedule();
    }
  }
}
void LampShow::playLampShow(Sequence sequence) {
  // If all workers are full, we cannot
  // start a new one else start one.
  //printf("In playLampShow...\n");
  int i = -1;
  for (int index = 0; index < 3; index++) {
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
    _pWorkerArray[i] = new MultiballLampShowWorker(this);
    break;

  case LampShow::Sequence::BallPlunged:
    break;

  case LampShow::Sequence::Jackpot:
    break;

  case LampShow::Sequence::SuperJackpot:
    break;
  }
}
