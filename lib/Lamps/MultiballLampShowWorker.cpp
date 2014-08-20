#include "MultiballLampShowWorker.h"
#include <stdio.h>

MultiballLampShowWorker::MultiballLampShowWorker(LampShow *pParentLampShow) : LampShowWorker(pParentLampShow) {
  _currentState = 1;
}

MultiballLampShowWorker::~MultiballLampShowWorker() {
  //dtor
}


void MultiballLampShowWorker::schedule() {
  // printf("In MultiballLampShowWorker::schedule...\n");
  LampShowWorker::schedule();
  switch (_currentState) {
    case 1 :
      _currentState++;
      break;
    case 2 :
      _currentState++;
      break;
    case 3 :
      _currentState++;
      break;
    case 4 :
      _currentState = 1;
      _pParentLampShow->finishedLampShow(this);
      break;
  }
}
