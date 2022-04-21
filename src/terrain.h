#pragma once

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "coords.h"
#include "utility.h"

#define CHUNKARR_DIAM 3
#define CHUNKARR_RAD  (CHUNKARR_DIAM - 1) / 2
#define CHUNKARR_SIZE CHUNKARR_DIAM * CHUNKARR_DIAM * CHUNKARR_DIAM
#define CHUNK_SIZE 64
#define CHUNK_DATA_SIZE CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

typedef u_int8_t      Block;
typedef struct _Chunk Chunk;
typedef struct _World World;

void  World_init     (World *);
void  World_sort     (World *);
int   World_setBlock (World *, int, int, int, Block, int);
Block World_getBlock (World *, int, int, int);

void Chunk_init (Chunk *);

Chunk *chunkLookup  (World *, int, int, int);
int    setCube      (World *, int, int, int, int, int, int, Block, int);
void   genStructure (World *, int, int, int, int);
int    genChunk     (World *, unsigned int, int, int, int, int, int, Coords);
void   genAround    (World *, Coords);

int  ch_setBlock   (Block *, int, int, int, Block);
void ch_genClassic (Block *);
void ch_genNew     (Block *, World *, unsigned int, int, int, int);
void ch_genStone   (Block *);
void ch_genFlat    (Block *);

/* _Chunk
 * Stores blocks in a chunk, a hash to make it easier to look up,
 * and a pointer to a block array.
 * 
 * coordHash will contain a hash of the coords to help look up the
 * chunk. When a new chunk is generated, its coords will be hashed
 * and its loaded will be set to true. Then, the chunk meta array
 * will be sorted by coordhash.
 */
struct _Chunk {
        IntCoords center;
        int coordHash;
        int loaded;
        Block *blocks;
};

/* _World
 * Stores chunks.
 */
struct _World {
        Chunk chunk[CHUNKARR_SIZE];
};
