#pragma once

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "structs.h"
#include "utility.h"

void   initChunks  (World*);
void   sortChunks  (World*);
Chunk* chunkLookup (World*, int, int, int);
int    setBlock    (World*, int, int, int, int, int);
int    getBlock    (World*, int, int, int);
int setCube(
  World*,
  int, int, int,
  int, int, int,
  int, int
);
int  ch_setBlock   (int*, int, int, int, int);
void genStructure  (World*, int, int, int, int);
int  genChunk(
  World*,
  unsigned int,
  int, int, int,
  int,
  int,
  Coords
);
