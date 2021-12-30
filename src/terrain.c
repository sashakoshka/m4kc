#include "terrain.h"

/*
  World_sort
  Sorts all chunks in a world by hash
*/
void World_sort (World* world) {
  int i, j;
  Chunk temp;
  for (i = 0; i < CHUNKARR_SIZE; i++)
  for (j = 0; j < (CHUNKARR_SIZE - 1 - i); j++)
  if (
    world->chunk[j].coordHash < world->chunk[j + 1].coordHash
  ) {
    temp = world->chunk[j];
    world->chunk[j] = world->chunk[j + 1];
    world->chunk[j + 1] = temp;
  }
}

/*
  chunkLookup
  Takes in a world pointer, and returns a pointer to the chunk
  at the specific x y and z coordinates. If the chunk is the
  same as last time, it does not do another lookup, meaning this
  function can be called very frequently.
*/
Chunk* chunkLookup (World *world, int x, int y, int z) {
  static Chunk *chunk;
  // Rather unlikely position. Not a coord because integers are
  // faster
  static IntCoords ago = {100000000, 100000000, 100000000};
  // Divide by 64
  x >>= 6;
  y >>= 6;
  z >>= 6;
  if(
    ago.x != x ||
    ago.y != y ||
    ago.z != z
  ) {
    ago.x = x;
    ago.y = y;
    ago.z = z;
    
    // Quickly hash the chunk coordinates
    
    // Since we already divided them by 64 (with bit shifting),
    // we can pick up from here.
    // Modulo-like operation by bitmasking
    x &= 0b1111111111;
    y &= 0b1111111111;
    z &= 0b1111111111;
    
    // Move these into their correct "slots"
    y <<= 10;
    z <<= 20;
    
    // Flatten them using binary or. Hash is stored in X.
    x |= y | z;
    x++;
    
    // Look up chunk using a binary search
    int first, middle, last;
    
    first  = 0,
    last   = CHUNKARR_SIZE - 1,
    middle = (CHUNKARR_SIZE - 1) / 2;
    
    while(first <= last) {
      if(world->chunk[middle].coordHash > x)
        first = middle + 1;
      else if(world->chunk[middle].coordHash == x) {
        chunk = &world->chunk[middle];
        return chunk;
      } else last = middle - 1;
      middle = (first + last) / 2;
    }
    chunk = NULL;
  }
  return chunk;
}

/*
  World_setBlock
  Takes in a world, xyz coordinates, and a block id.
  Returns true if the block could be set, otherwise returns
  false. Eventually will add block to a stack of set requests if
  the chunk is not loaded, and will set the block when the chunk
  loads. If force is true, blocks other than air will be set.
*/
int World_setBlock (
  World *world,
  int x, int y, int z,
  int block,
  int force
) {
  static int   b;
  static Chunk *chunk;
  b = World_getBlock(world, x, y, z) < 1;
  
  if(force || b) { // If the block was air or we don't care
    chunk = chunkLookup(world, x, y, z);
    
    // If chunk does not have an allocated block array, exit
    if(chunk == NULL || !chunk->loaded) return -1;
    
    chunk->blocks[
      nmod(x, 64) + (nmod(y, 64) << 6) + (nmod(z, 64) << 12)
    ] = block;
    return b;
  } else {
    return 0;
  }
}

/*
  World_getBlock
  Takes in a world, xyz coordinates, and outputs the block
  id there. Returns -1 if chunk is not loaded
*/
int World_getBlock (
  World *world,
  int x, int y, int z
) {
  static Chunk *chunk;
  chunk = chunkLookup(world, x, y, z);
  
  // If chunk does not have an allocated block array, exit
  if(chunk == NULL || !chunk->loaded) return -1;
  
  return chunk->blocks[
    nmod(x, 64) + (nmod(y, 64) << 6) + (nmod(z, 64) << 12)
  ];
}

/*
  ch_setBlock
  Takes in a blocks array, xyz coordinates, and a block id.
  Sets the block. For usage in terrain generation. Returns false
  if the block was previously air.
*/
int ch_setBlock (
  int *blocks,
  int x, int y, int z,
  int block
) {
  static int b;
  b = blocks[
    nmod(x, 64) + (nmod(y, 64) << 6) + (nmod(z, 64) << 12)
  ] > 0;
  blocks[
    x + (y << 6) + (z << 12)
  ] = block;
  return b;
}

/*
  setCube
  Takes in a world array, xyz coordinates, dimensions, and fills
  in a cube with the specified block. If force is true, blocks
  other than air will be filled. If no blocks were previously
  air, returns false.
*/
int setCube (
  World *world,
  int x, int y, int z,
  int w, int h, int l,
  int block,
  int force
) {
  static int xx, yy, zz, b;
  x--; y--; z--; b = 0;
  for(xx = w + x; xx > x; xx--)
  for(yy = h + y; yy > y; yy--)
  for(zz = l + z; zz > z; zz--)
    b |= World_setBlock(world, xx, yy, zz, block, force);
  return b;
}

/*
  genStructure
  Takes in a world array, xyz coordinates, and generates the
  specified structure.
*/
void genStructure (
  World *world,
  int x, int y, int z,
  int type
) {
  static int i, b;
  
  /*
    Structure ideas
    
    * obelisks
    * villages
    * giant trees with villages in them
    * brick houses
    * statues
    * mineshafts
    * bridges
    * fortresses
    
  */
  switch(type) {
    case 0: // tree
      for(i = randm(2) + 4; i > 0; i--) {
        World_setBlock(world, x, y--, z, 7, 1);
      }
      setCube(world, x - 2, y + 1, z - 2, 5, 2, 5, 8, 0);
      setCube(world, x - 1, y - 1, z - 1, 3, 2, 3, 8, 0);
      break;
    
    case 1: // pyramid
      y -= 5 + randm(2);
      b = 1;
      for(i = 1; b > 0 && i < 64; i+= 2)
        b &= setCube(
          world,
          x - i / 2,
          y++,
          z - i / 2,
          i, 1, i,
          5, 1
        );
      break;
  }
}

/*
  genChunk
  Takes in a seed and a chunk array. Chunk is 64x64x64 blocks.
  Fills the chunk array with generated terrain. If force is set
  to true, a chunk at the same coordinates will be overwritten.
*/
int genChunk (
  World *world,
  unsigned int seed,
  int xOffset,
  int yOffset,
  int zOffset,
  int type,
  int force,
  Coords coords
) {
  xOffset = (xOffset / 64) * 64;
  yOffset = (yOffset / 64) * 64;
  zOffset = (zOffset / 64) * 64;
  // To make sure structure generation accross chunks is
  // different, but predictable
  srand(seed * (xOffset * yOffset * zOffset + 1));
  int heightmap[64][64], i, x, z, /*loadedMin,*/ distMax, distMaxI;
  static int count = 0;
  
  Chunk *chunk = chunkLookup(world, xOffset, yOffset, zOffset);
  
  // Only generate if that chunk hasn't been generated yet.
  i = 0;
  if(chunk == NULL) {
    // See if there is an empty slot
    for(; i < CHUNKARR_SIZE && world->chunk[i].loaded; i++);
  
    // Pick out the oldest chunk (loaded) and overrwrite it.
    /* TODO: Find oldest chunk by which one is farthest away from
    the player */
    if(i == CHUNKARR_SIZE) {
      /*
      loadedMin = world->chunk[0].loaded;
      for(i = 0; i < CHUNKARR_SIZE; i++)
        if(
          world->chunk[i].loaded <
          world->chunk[loadedMin].loaded
        ) loadedMin = i;
      i = loadedMin;
      */
      distMax  = 0;
      distMaxI = 0;
      for(i = 0; i < CHUNKARR_SIZE; i++) {
        int dist = sqrt(
          pow(coords.x - world->chunk[i].center.x, 2) +
          pow(coords.y - world->chunk[i].center.y, 2) +
          pow(coords.z - world->chunk[i].center.z, 2)
        );
        if(dist > distMax) {
          distMax  = dist;
          distMaxI = i;
        }
      }
      i = distMaxI;
    }
    chunk = &world->chunk[i];
  } else if(!force) {
    return 0;
  }
  
  // If there is no array, allocate one.
  if(chunk->loaded) {
    // TODO: Save chunk to disk
  } else {
    chunk->blocks = (int*)calloc(262144, sizeof(int));
  }
  
  if(chunk->blocks == NULL) {
    printf("genChunk: memory allocation fail\n");
    return 0;
  }
  
  int *blocks = chunk->blocks;
  
  for(int i = 0; i < 262144; i++)
    blocks[i] = 0;
  
  // Generate a hash
  static int hashX, hashY, hashZ;
  
  hashX = xOffset >> 6;
  hashY = yOffset >> 6;
  hashZ = zOffset >> 6;
  
  hashX &= 0b1111111111;
  hashY &= 0b1111111111;
  hashZ &= 0b1111111111;
  
  hashY <<= 10;
  hashZ <<= 20;
  
  chunk->coordHash = hashX | hashY | hashZ;
  chunk->coordHash++;
  
  chunk->center.x = xOffset + 32;
  chunk->center.y = xOffset + 32;
  chunk->center.z = xOffset + 32;
  
  // What we have here won't cause a segfault, so it is safe to
  // mark the chunk as loaded and set its stamp.
  chunk->loaded = ++count;
  
  printf(
    "chunk hash: %#016x\tx: %i\ty: %i\tz: %i\tstamp: %i\taddr: %p\tgenerated\n",
    chunk->coordHash,
    xOffset, yOffset, zOffset,
    chunk->loaded, chunk
  );
  
  
  switch(type) {
    // Classic terrain
    case 0:
      for(int x = 0; x < 64; x++)
        for(int y = 32; y < 64; y++)
          for(int z = 0; z < 64; z++)
            ch_setBlock(blocks, x, y, z,
              randm(2) == 0 ? randm(8) : 0);
      break;
    
    // New terrain
    case 1:
      // Generate heightmap
      for(int x = 0; x < 64; x++)
        for(int z = 0; z < 64; z++) {
          heightmap[x][z] =
            perlin2d( // Detail noise
              seed,
              x + xOffset + 16777215,
              z + zOffset + 16777215,
              0.0625
            )
            * 16 +
            perlin2d( // Detail noise
              seed,
              x + xOffset + 16777215,
              z + zOffset + 16777215,
              0.0078125
            )
            * 64;
        }
      
      // Make terrain from heightmap
      for(int x = 0; x < 64; x++)
        for(int y = 0; y < 64; y++)
          for(int z = 0; z < 64; z++)
            if(y + yOffset > heightmap[x][z] + 4)
              ch_setBlock(blocks, x, y, z, 4);
            else if(y + yOffset > heightmap[x][z])
              ch_setBlock(blocks, x, y, z, 2);
            else if(y + yOffset == heightmap[x][z])
              ch_setBlock(blocks, x, y, z, 1);
            else
              ch_setBlock(blocks, x, y, z, 0);
      
      // Generate structures
      // TODO: fix structures not properly generating in some
      // chunks
      for(i = randm(16) + 64; i > 0; i--) {
        x = randm(64);
        z = randm(64);
        if(
          heightmap[x][z] >= yOffset &&
          heightmap[x][z] <  yOffset + 64
        ) {
          genStructure(
            world,
            x + xOffset, heightmap[x][z] - 1, z + zOffset,
            0
          );
        }
      }
      
      for(i = randm(2); i > 0; i--) {
        x = randm(64);
        z = randm(64);
        if(
          heightmap[x][z] >= yOffset &&
          heightmap[x][z] <  yOffset + 64
        ) {
          genStructure(
            world,
            x + xOffset, heightmap[x][z] + 1, z + zOffset,
            1
          );
        }
      }
      
      break;
    
    // Debug stone
    case 2:
      for(int x = 0; x < 64; x++)
        for(int z = 0; z < 64; z++) {
          for(int y = 0; y < 32; y++)
            ch_setBlock(blocks, x, y, z, 4);
          for(int y = 32; y < 64; y++)
            ch_setBlock(blocks, x, y, z, 5);
        }

    // Flat
    case 3:
      for(int x = 0; x < 64; x++)
        for(int z = 0; z < 64; z++) {
          for(int y = 0; y < 64; y++) {
            if (y < 32)  ch_setBlock(blocks, x, y, z, 0);
            if (y == 32) ch_setBlock(blocks, x, y, z, 1);
            if (y > 32)  ch_setBlock(blocks, x, y, z, 2);
          }
        }
      break;
  }
  
  // Sort all chunks
  World_sort(world);
  return 1;
}

void World_init (World *world) {  
  for (int i = 0; i < CHUNKARR_SIZE; i++)
    Chunk_init(&world->chunk[i]);
}

void Chunk_init (Chunk *chunk) {
  chunk->center.x  = 0;
  chunk->center.y  = 0;
  chunk->center.z  = 0;
  chunk->coordHash = 0;
  chunk->loaded    = 0;
  chunk->blocks    = NULL;
}
