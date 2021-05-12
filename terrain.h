void   initChunks  (World*);
void   genAll      (World*, unsigned int, int);
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
void genChunk(
  World*,
  unsigned int,
  int, int, int,
  int
);

/*
  initChunks
  Initializes all chunk slots
*/
void initChunks(World *world) {
  static int i;
  for(i = 0; i < 27; i++) {
    world->chunk[i].coordHash = 0;
    world->chunk[i].loaded    = 0;
    world->chunk[i].blocks    = NULL;
  }
}

/*
  genAll
  generates all chunks surrounding the player. TODO: once chunk
  indexing system fully working, call this when the player
  crosses chunk boundaries
*/
void genAll(World *world, unsigned int seed, int type) {
  // For all chunk slots we have, go around the player. This
  // will eventually take in player position.
  for(int x = 0; x < 64 * 3; x += 64)
  for(int y = 0; y < 64 * 3; y += 64)
  for(int z = 0; z < 64 * 3; z += 64)
    genChunk(world, seed, x, y, z, type);
}
/*
  chunkLookup
  Takes in a world pointer, and returns a pointer to the chunk
  at the specific x y and z coordinates. If the chunk is the same
  as last time, it does not do another lookup, meaning this
  function can be called very frequently.
*/
Chunk* chunkLookup(World *world, int x, int y, int z) {
  static Chunk *chunk;
  // Rather unlikely position. Not a coord because integers are
  // faster
  static int lastX = 100000000;
  static int lastY = 100000000;
  static int lastZ = 100000000;
  // Divide by 64
  x >>= 6;
  y >>= 6;
  z >>= 6;
  if(
    lastX != x ||
    lastY != y ||
    lastZ != z
  ) {
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
    
    // TODO: look up chunk instead of this. If chunk is not
    // found, return null.
    
    if(x == 0 && y == 0 && z == 0)
      chunk = &world->chunk[0];
    else
      return NULL;
    
    lastX = x;
    lastY = y;
    lastZ = z;
  }
  return chunk;
}

/*
  setBlock
  Takes in a world array, xyz coordinates, and a block id.
  Returns true if the block could be set, otherwise returns
  false. Eventually will add block to a stack of set requests if
  the chunk is not loaded, and will set the block when the chunk
  loads. If force is true, blocks other than air will be set.
*/
int setBlock(
  World *world,
  int x, int y, int z,
  int block,
  int force
) {
  static int   b;
  static Chunk *chunk;
  b = getBlock(world, x, y, z) < 1;
  
  if(force || b) { // If the block was air or we don't care
    chunk = chunkLookup(world, x, y, z);
    
    // If chunk does not have an allocated block array, exit
    if(chunk == NULL || !chunk->loaded) return -1;
    
    chunk->blocks[ + x + y * 64 + z * 4096] = block;
    return b;
  } else {
    return 0;
  }
}

/*
  getBlock
  Takes in a world array, xyz coordinates, and outputs the block
  id there. Returns -1 if chunk is not loaded
*/
int getBlock(
  World *world,
  int x, int y, int z
) {
  static Chunk *chunk;
  chunk = chunkLookup(world, x, y, z);
  
  // If chunk does not have an allocated block array, exit
  if(chunk == NULL || !chunk->loaded) return -1;
  
  return chunk->blocks[
    x +
    y * 64 +
    z * 4096
  ];
}

/*
  ch_setBlock
  Takes in a blocks array, xyz coordinates, and a block id.
  Sets the block. For usage in terrain generation. Returns false
  if the block was previously air.
*/
int ch_setBlock(
  int *blocks,
  int x, int y, int z,
  int block
) {
  static int b;
  b = blocks[x + y * 64 + z * 4096] > 0;
  blocks[x + y * 64 + z * 4096] = block;
  return b;
}

/*
  setCube
  Takes in a world array, xyz coordinates, dimensions, and fills
  in a cube with the specified block. If force is true, blocks
  other than air will be filled. If no blocks were previously
  air, returns false.
*/
int setCube(
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
    b |= setBlock(world, xx, yy, zz, block, force);
  return b;
}

/*
  genStructure
  Takes in a world array, xyz coordinates, and generates the
  specified structure.
*/
void genStructure(
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
        setBlock(world, x, y--, z, 7, 1);
      }
      setCube(world, x - 2, y + 1, z - 2, 5, 2, 5, 8, 0);
      setCube(world, x - 1, y - 1, z - 1, 3, 2, 3, 8, 0);
      break;
    
    case 1: // pyramid
      y -= 5 + randm(2);
      b = 1;
      for(i = 1; b > 0; i+= 2)
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
  Fills the chunk array with generated terrain.
*/
void genChunk(
  World *world,
  unsigned int seed,
  int xOffset,
  int yOffset,
  int zOffset,
  int type
) {
  
  // To make sure structure generation accross chunks is
  // different, but predictable
  srand(seed * (xOffset * yOffset * zOffset + 1));
  int heightmap[64][64], i, x, z;
  
  Chunk *chunk = chunkLookup(world, xOffset, yOffset, zOffset);
  
  if(chunk == NULL) return;
  
  // If there is no array, allocate one.
  if(!chunk->loaded)
    chunk->blocks = (int*)calloc(262144, sizeof(int));
  else {
    // TODO: Save chunk to disk
  }
  
  if(chunk->blocks == NULL) {
    printf("genChunk: memory allocation fail");
    return;
  }
  
  int *blocks = chunk->blocks;
  
  for(int i = 0; i < 262144; i++)
    blocks[i] = 19;
  
  // Generate a hash
  xOffset >>= 6;
  yOffset >>= 6;
  zOffset >>= 6;
  
  xOffset &= 0b1111111111;
  yOffset &= 0b1111111111;
  zOffset &= 0b1111111111;
  
  yOffset <<= 10;
  zOffset <<= 20;
  
  chunk->coordHash = xOffset | yOffset | zOffset;
  
  // What we have here won't cause a segfault, so it is safe to
  // mark the chunk as loaded.
  chunk->loaded = 1;
  
  switch(type) {
    case 0:
      for(int x = 0; x < 64; x++)
        for(int y = 32; y < 64; y++)
          for(int z = 0; z < 64; z++)
            ch_setBlock(blocks, x, y, z,
              randm(2) == 0 ? randm(8) : 0);
      break;
    case 1:
      // Generate heightmap
      for(int x = 0; x < 64; x++)
        for(int z = 0; z < 64; z++) {
          heightmap[x][z] =
            perlin2d(x, z, seed) * 16 + 24;
        }
      
      // Make terrain from heightmap
      for(int x = 0; x < 64; x++)
        for(int y = 0; y < 64; y++)
          for(int z = 0; z < 64; z++)
            if(y > heightmap[x][z] + 4)
              ch_setBlock(blocks, x, y, z, 4);
            else if(y > heightmap[x][z])
              ch_setBlock(blocks, x, y, z, 2);
            else if(y == heightmap[x][z])
              ch_setBlock(blocks, x, y, z, 1);
            else
              ch_setBlock(blocks, x, y, z, 0);
      
      // Generate structures
      for(i = randm(16) + 64; i > 0; i--) {
        x = randm(64);
        z = randm(64);
        genStructure(
          world,
          x, heightmap[x][z] - 1, z,
          0
        );
      }
      
      for(i = randm(4); i > 0; i--) {
        x = randm(64);
        z = randm(64);
        genStructure(
          world,
          x, heightmap[x][z] + 1, z,
          1
        );
      }
      
      break;
  }
}
