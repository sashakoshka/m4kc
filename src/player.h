#pragma once

#include <stdlib.h>
#include "coords.h"
#include "blocks.h"

#define HOTBAR_SIZE    9
#define ARMOR_SIZE     4
#define INVENTORY_ROWS 3
#define INVENTORY_SIZE HOTBAR_SIZE * INVENTORY_ROWS

/* _InvSlot
 * This will be used to store a single stack in the player's
 * inventory.
 */
typedef struct InvSlot {
        Block     blockid;
        u_int8_t  amount;
        u_int16_t durability;
} InvSlot;

int  InvSlot_transfer (InvSlot *, InvSlot *);
void InvSlot_swap     (InvSlot *, InvSlot *);

/* _Inventory
 * This will be used to store the player's inventory.
 */
typedef struct Inventory {
        InvSlot offhand;
        InvSlot hotbar[HOTBAR_SIZE];
        InvSlot slots[INVENTORY_SIZE];
        InvSlot armor[ARMOR_SIZE];
        int hotbarSelect;
} Inventory;

int Inventory_transferIn (Inventory *, InvSlot *);

/* _Player
 * Stores player data. This will be passed as reference to game
 * loop.
 */
typedef struct Player {
        Coords pos;
        float hRot;
        float vRot;

        Inventory inventory;
        u_int8_t  health;
        u_int8_t  hunger;
        u_int8_t  breath;
        u_int16_t xp;

        Vector2D vectorH;
        Vector2D vectorV;

        float FBVelocity;
        float LRVelocity;
} Player;

int Player_save (Player *, const char *);
int Player_load (Player *, const char *);
