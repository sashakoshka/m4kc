typedef struct _World     World;
typedef struct _Player    Player;
typedef struct _InvSlot   InvSlot;
typedef struct _Inventory Inventory;
typedef struct _Coords    Coords;

/*
  _World
  Stores a chunk. This will eventually store multiple of them.
*/
struct _World {
  int chunk[262144];
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
