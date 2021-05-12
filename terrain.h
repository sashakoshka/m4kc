void initChunks    (World*);
Chunk* chunkLookup (World*, int, int, int); 
int    setBlock    (World*, int, int, int, int, int);
int    getBlock    (World*, int, int, int);
int    setCube(
  World*,
  int, int, int,
  int, int, int,
  int, int
);
int    ch_setBlock(int*, int, int, int, int);
void   genStructure(World*, int, int, int, int);
void   genChunk(
  unsigned int,
  World*,
  int, int, int,
  int
);

/*
  initChunks
  Initializes all chunk slots
*/
void initChunks(World *world) {
  static int i;
  for(i = 0; i < 2; i++) {
    world->chunk[i].coordHash = 0;
    world->chunk[i].loaded    = 0;
    world->chunk[i].blocks    = NULL;
  }
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
  // Rather unlikely position
  static Coords last = {100000000, 100000000, 100000000};
  // Divide by 64
  x >>= 6;
  y >>= 6;
  z >>= 6;
  if(
    last.x != x ||
    last.y != y ||
    last.z != z
  ) {
    // TODO: look up chunk instead of this
    if(x == 0 && y == 0 && z == 0)
      chunk = &world->chunk[0];
    else
      chunk = &world->chunk[1];
    
    last.x = x;
    last.y = y;
    last.z = z;
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
    if(!chunk->loaded) return 0;
    
    chunk->blocks[ + x + y * 64 + z * 4096] = block;
    return b;
  } else {
    return 0;
  }
}

/*
  getBlock
  Takes in a world array, xyz coordinates, and outputs the block
  id there. Eventually will return -1 if chunk is not loaded
*/
int getBlock(
  World *world,
  int x, int y, int z
) {
  static Chunk *chunk;
  chunk = chunkLookup(world, x, y, z);
  
  // If chunk does not have an allocated block array, exit
  if(!chunk->loaded) return 0;
  
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
  unsigned int seed,
  World *world,
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
  
  // If there is no array, allocate one.
  if(!chunk->loaded)
    chunk->blocks = (int*)calloc(262144, sizeof(int));
  
  if(chunk->blocks == NULL) {
    printf("genChunk: memory allocation fail");
    return;
  }
  
  int *blocks = chunk->blocks;
  
  for(int i = 0; i < 262144; i++)
    blocks[i] = 19;
  
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
