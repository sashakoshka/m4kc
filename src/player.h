#pragma once

#include <stdlib.h>
#include "coords.h"

typedef struct _Player    Player;
typedef struct _InvSlot   InvSlot;
typedef struct _Inventory Inventory;

/*
  _InvSlot
  This will be used to store a single stack in the player's
  inventory.
*/
struct _InvSlot {
  float durability;
  u_int8_t amount;
  unsigned int blockid;
};

/*
  _Inventory
  This will be used to store the player's inventory.
*/
struct _Inventory {
  InvSlot slots[27];
  InvSlot hotbar[9];
  InvSlot armor[4];
  InvSlot offhand;
  int hotbarSelect;
};

/*
  _Player
  Stores player data. This will be passed as reference to game
  loop.
*/
struct _Player {
  Coords pos;
  Inventory inventory;
  float hRot;
  float vRot;
  
  unsigned int health;
  unsigned int xp;
};
