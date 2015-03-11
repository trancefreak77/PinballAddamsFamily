#include "LampShowWorker.h"

LampShowWorker::LampShowWorker(LampShow *pParentLampShow, bool shouldLoop) {
  _pParentLampShow = pParentLampShow;
  _shouldLoop = shouldLoop;
}

LampShowWorker::~LampShowWorker() {
  //dtor
}

void LampShowWorker::schedule() {}

bool LampShowWorker::getShouldLoop() {
  return _shouldLoop;
}

std::map<uint8_t, bool>& LampShowWorker::getUnusedLampNumbers() {
  return _unusedLampsMap;
}
