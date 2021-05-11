typedef struct _ChunkMeta ChunkMeta;
typedef struct _World     World;
typedef struct _Player    Player;
typedef struct _InvSlot   InvSlot;
typedef struct _Inventory Inventory;
typedef struct _Coords    Coords;

/*
  _ChunkMeta
  Accompanies a chunk array in a World. The World should have an
  array of these, and an array of chunk arrays. This will point to
  its proper chunk array. The purpose of this struct is to store
  data about a chunk that makes it easier to look up, and be
  movable in memory (eventual sorting for binary search)
  
  coordHash will contain a hash of the coords to help look up the
  chunk.
*/
struct _ChunkMeta {
  int loaded;
  int coordHash;
  int *chunk[];
};

/*
  _World
  Stores a chunk. This will eventually store multiple of them.
*/
// 25: amount of chunks that can be loaded at one time
struct _World {
  int       chunk[262144];
  ChunkMeta meta [25];
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
