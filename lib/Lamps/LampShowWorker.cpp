#include "LampShowWorker.h"

LampShowWorker::LampShowWorker(LampShow *pParentLampShow) {
  _pParentLampShow = pParentLampShow;
}

LampShowWorker::~LampShowWorker() {
  //dtor
}

void LampShowWorker::schedule() {}
