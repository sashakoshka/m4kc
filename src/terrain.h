#pragma once

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include "blocks.h"
#include "coords.h"
#include "utility.h"

#define CHUNKARR_DIAM 3
#define CHUNKARR_RAD  (CHUNKARR_DIAM - 1) / 2
#define CHUNKARR_SIZE CHUNKARR_DIAM * CHUNKARR_DIAM * CHUNKARR_DIAM
#define CHUNK_SIZE 64
#define CHUNK_DATA_SIZE CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

typedef struct _Chunk Chunk;
typedef struct _World World;

void  World_sort     (World *);
int   World_save     (World *);
int   World_load     (World *, const char *path);
int   World_setBlock (World *, int, int, int, Block, int);
Block World_getBlock (World *, int, int, int);

u_int32_t chunkHash (int, int, int);
Chunk *chunkLookup  (World *, int, int, int);
int    genChunk     (World *, unsigned int, int, int, int, int, int, Coords);
void   genStructure (Block *, int, int, int, int);
void   genAround    (World *, Coords);

int  ch_setCube    (Block *, int, int, int, int, int, int, Block, int);
int  ch_setBlock   (Block *, int, int, int, Block, int);
void ch_genClassic (Block *, int);
void ch_genNew     (Block *, unsigned int, int, int, int);
void ch_genStone   (Block *, int);
void ch_genFlat    (Block *, int);
void ch_genWater   (Block *, int);
void ch_genDev     (Block *, int, int, int);

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
        u_int32_t coordHash;
        int loaded;
        Block *blocks;
};

/* _World
 * Stores chunks.
 */
struct _World {
        int       type;
        u_int64_t seed;
        int       dayNightMode;
        u_int64_t time;
        char      path[PATH_MAX];
        Chunk chunk[CHUNKARR_SIZE];
};
