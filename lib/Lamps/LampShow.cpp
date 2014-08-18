#include "LampShow.h"
#include <stdio.h>
#include <stdlib.h>
#include <propeller.h>

LampShow::LampShow() {
  //ctor
}

LampShow::~LampShow() {
  //dtor
}
void LampShow::schedule() {
  Scheduler::schedule();
}
void LampShow::playLampShow(const char filename[]) {
  // Load the file from sd card and parse it.
  // The first line in the file tells us, how many
  // lamps are defined within the file.
  printf("In playLampShow...\n");
  char buffer[1024];
  int startCNT = CNT;
  FILE *fp = fopen(filename, "r");
//  if (!fp) {
//    // Unable to open file. Lamp show cannot
//    // be played.
//    printf("Unable to open file...\n");
//    return;
//  }

  // Read the number of lamps
//  if (!fgets(buffer, 512, fp)) {
//    // Error or nothing to read.
//    printf("Error or nothing to read...\n");
//    return;
//  }

  // Parse the number of lamps from char*
  // int numberOfLamps = atoi(buffer);
  // printf("Buffer: %d\n", numberOfLamps);

//  // Buffer contains now the first line.
//  // 1. Schritt: Dynamische Array von Zeigern anlegen:
//  char** p2DimArr = new char*[numberOfLamps];
//
//  // 2. Schritt: An jeden Zeiger ein Array hängen
//  for (int i = 0; i < numberOfLamps ; i++)
//    p2DimArr[i] = new char[100];
//
//  // mit dem neuen 2-dimensionalen Array arbeiten
//  p2DimArr[0][0] = 0xFF;
//  //...

  fread(buffer, 1, 1024, fp);

  // alles wieder löschen
//  for (int j = 0; j < numberOfLamps ; j++)
//    delete [] p2DimArr[j] ;
//  delete [] p2DimArr;

  int endCNT = CNT;

  printf("Buffer: ");
  printf(buffer);
  printf("\n");
  printf("Lamp show array created. Ticks: %d\n", endCNT - startCNT);
}
