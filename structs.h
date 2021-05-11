typedef struct _Chunk     Chunk;
typedef struct _World     World;
typedef struct _Player    Player;
typedef struct _InvSlot   InvSlot;
typedef struct _Inventory Inventory;
typedef struct _Coords    Coords;

/*
  _Chunk
  Stores blocks in a chunk, a hash to make it easier to look up,
  and a pointer to a block array.
  
  coordHash will contain a hash of the coords to help look up the
  chunk. When a new chunk is generated, its coords will be hashed
  and its loaded will be set to true. Then, the chunk meta array
  will be sorted by coordhash.
*/
struct _Chunk {
  int coordHash;
  int loaded;
  int *blocks[];
};

/*
  _World
  Stores a chunk. This will eventually store multiple of them.
*/
// 25: amount of chunks that can be loaded at one time
struct _World {
  int   tempBlocks [262144];
  Chunk chunk      [25];
};

/*
  _Player
  Stores player data. This will be passed as reference to game
  loop.
*/
struct _Player {
  float xPos;
  float yPos;
  float zPos;
  float xRot;
  float yRot;
  float zRot;
  
  unsigned int health;
  unsigned int xp;
};

/*
  _InvSlot
  This will be used to store a single stack in the player's
  inventory.
*/
struct _InvSlot {
  unsigned int amount:6;
  unsigned int blockid;
} pack;

/*
  _Inventory
  This will be used to store the player's inventory.
*/
struct _Inventory {
  InvSlot slots[27];
  InvSlot hotbar[9];
  InvSlot armor[4];
};

/*
  _Coords
  Stores xyz coordinates
*/
struct _Coords {
  int x;
  int y;
  int z;
};
