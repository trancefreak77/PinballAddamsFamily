#include "LampShow.h"
#include <stdio.h>

LampShow::LampShow() {
  //ctor
}

LampShow::~LampShow() {
  //dtor
}
void LampShow::schedule() {
  Scheduler::schedule();
}
void LampShow::playLampShow(char filename[]) {
  // Load the file from sd card and parse it.
  // The first line in the file tells us, how many
  // lamps are defined within the file.
  char buffer[512];
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    // Unable to open file. Lamp show cannot
    // be played.
    return;
  }

  // Read the number of lamps
  if (fgets(buffer, 512, fp) == NULL) {
    // Error or nothing to read.
    return;
  }

  // Buffer contains now the first line.
}
