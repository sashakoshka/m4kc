int*  chunkLookup(World*, int, int, int); 
int   setBlock(World*, int, int, int, int, int);
int   getBlock(World*, int, int, int);
int   setCube(
  World*,
  int, int, int,
  int, int, int,
  int, int
);
int   ch_setBlock(int*, int, int, int, int);
void  genStructure(World*, int, int, int, int);
void  genChunk(
  unsigned int,
  World*,
  int, int, int,
  int
);

/*
  chunkLookup
  Takes in a world pointer, and returns a pointer to the chunk
  at the specifiec x y and z coordinates. If the chunk is the
  same as last time, it does not do another lookup, meaning this
  function can be called very frequently.
*/
int* chunkLookup(World* world, int x, int y, int z) {
  // Rather unlikely position
  static Coords last = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
  static int *chunk;
  // Divide by 64
  x >>= 6;
  y >>= 6;
  z >>= 6;
  if(
    last.x != x &&
    last.y != y &&
    last.z != z
  ) {
    // TODO: look up chunk instead of this
    chunk = world->chunk[0];
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
  static int b;
  b = getBlock(world, x, y, z) < 1;
  
  if  (x > -1 && x < 64
    && y > -1 && y < 64
    && z > -1 && z < 64
    && (force || b)
  ) {
    chunkLookup(world, x, y, z)[x + y * 64 + z * 4096] = block;
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
  if (x < 0
    || y < 0
    || z < 0
    || x >= 64
    || y >= 64
    || z >= 64
  ) return 0;
  return chunkLookup(world, x, y, z)[
    x +
    y * 64 +
    z * 4096
  ];
}

/*
  ch_setBlock
  Takes in a chunk array, xyz coordinates, and a block id.
  Sets the block. For usage in terrain generation. Returns false
  if the block was previously air.
*/
int ch_setBlock(
  int *chunk,
  int x, int y, int z,
  int block
) {
  static int b;
  b = chunk[x + y * 64 + z * 4096] > 0;
  chunk[x + y * 64 + z * 4096] = block;
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
  static int heightmap[64][64], i, x, z;
  
  int *chunk = chunkLookup(world, xOffset, yOffset, zOffset);
  
  for(int i = 0; i < 262144; i++)
    chunk[i] = 0;
  
  switch(type) {
    case 0:
      for(int x = 0; x < 64; x++)
        for(int y = 32; y < 64; y++)
          for(int z = 0; z < 64; z++)
            ch_setBlock(chunk, x, y, z,
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
              ch_setBlock(chunk, x, y, z, 4);
            else if(y > heightmap[x][z])
              ch_setBlock(chunk, x, y, z, 2);
            else if(y == heightmap[x][z])
              ch_setBlock(chunk, x, y, z, 1);
            else
              ch_setBlock(chunk, x, y, z, 0);
      
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
