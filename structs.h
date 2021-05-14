typedef struct _Chunk     Chunk;
typedef struct _World     World;
typedef struct _Player    Player;
typedef struct _InvSlot   InvSlot;
typedef struct _Inventory Inventory;
typedef struct _Coords    Coords;
typedef struct _IntCoords IntCoords;
typedef struct _Inputs    Inputs;

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
  int *blocks;
};

/*
  _World
  Stores a chunk. This will eventually store multiple of them.
*/
// 25: amount of chunks that can be loaded at one time
struct _World {
  Chunk chunk[27];
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
  float x;
  float y;
  float z;
};

/*
  _IntCoords
  Stores xyz integer coordinates
*/
struct _IntCoords {
  int x;
  int y;
  int z;
};

/*
  _Inputs
  Stores the state of the different inputs (keyboard/mouse) for
  the current tick. Updated on each game loop.
*/
struct _Inputs {
  int mouse_Right;     // M[0]
  int mouse_Left;      // M[1]
  int mouse_X;         // M[2]
  int mouse_Y;         // M[3]
  int mouse_Wheel;     // M[4]

  int keyboard_Space;  // M[32]
  int keyboard_W;      // M[119]
  int keyboard_S;      // M[115]
  int keyboard_A;      // M[97]
  int keyboard_D;      // M[100]

  int keyboard_Esc;    // M[27]
  int keyboard_F1;     // M[5]
  int keyboard_F2;     // M[6]
  int keyboard_F3;     // M[7]
};
