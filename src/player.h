#pragma once

#include <stdlib.h>
#include "coords.h"
#include "terrain.h"

#define HOTBAR_SIZE 9
#define INVENTORY_ROWS 3
#define INVENTORY_SIZE HOTBAR_SIZE * INVENTORY_ROWS

typedef struct _Player    Player;
typedef struct _InvSlot   InvSlot;
typedef struct _Inventory Inventory;

/* _InvSlot
 * This will be used to store a single stack in the player's
 * inventory.
 */
struct _InvSlot {
        float durability;
        u_int8_t amount;
        Block blockid;
};

int  InvSlot_transfer (InvSlot *, InvSlot *);
void InvSlot_swap     (InvSlot *, InvSlot *);

/* _Inventory
 * This will be used to store the player's inventory.
 */
struct _Inventory {
        InvSlot slots[INVENTORY_SIZE];
        InvSlot hotbar[HOTBAR_SIZE];
        InvSlot armor[4];
        InvSlot offhand;
        int hotbarSelect;
};

int Inventory_transferIn (Inventory *, InvSlot *);

/* _Player
 * Stores player data. This will be passed as reference to game
 * loop.
 */
struct _Player {
        Coords pos;
        Inventory inventory;
        float hRot;
        float vRot;

        float FBVelocity;
        float LRVelocity;

        unsigned int health;
        unsigned int xp;
};
