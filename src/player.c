#include <stdlib.h>
#include <stdio.h>
#include "player.h"

static void Inventory_save (FILE *, Inventory *);
static void Inventory_load (FILE *, Inventory *);
static void InvSlot_save   (FILE *, InvSlot *);
static void InvSlot_load   (FILE *, InvSlot *);

/* Player_save
 * Saves a player file to the specified path. Returns 0 on success, non-zero on
 * failure.
 */
int Player_save (Player *player, const char *path) {
        FILE *file = fopen(path, "w");
        if (file == NULL) { return 1; }

        fprintf (
                file,
                "%i\n"
                "%a %a %a\n"
                "%a %a\n"
                "%hhu %hhu %hhu\n"
                "%hu\n\n",
                0,
                player->pos.x, player->pos.y, player->pos.z,
                player->hRot, player->vRot,
                player->health, player->hunger, player->breath,
                player->xp);

        Inventory_save(file, &player->inventory);
        
        fclose(file);
        return 0;
}

/* Player_save
 * Loads a player from the specified file. Returns 0 on success, non-zero on
 * failure.
 */
int Player_load (Player *player, const char *path) {
        FILE *file = fopen(path, "r");
        if (file == NULL) { return 1; }
        
        int version;
        fscanf(file, "%i", &version);
        if (version != 0) { return 2; }

        fscanf (
                file,
                "%lf %lf %lf "
                "%lf %lf "
                "%hhu %hhu %hhu "
                "%hu",
                &player->pos.x, &player->pos.y, &player->pos.z,
                &player->hRot, &player->vRot,
                &player->health, &player->hunger, &player->breath,
                &player->xp);

        Inventory_load(file, &player->inventory);
        
        fclose(file);
        return 0;
}

/* Inventory_transferIn
 * Transfers items from src int dest. This will transfer as many items as it
 * can. Returns true if all items were transferred, false if some items weren't.
 */
int Inventory_transferIn (Inventory *dest, InvSlot *src) {
        // Look for exact match in hotbar
        for (int i = 0; i < HOTBAR_SIZE; i ++) {
                if(dest->hotbar[i].blockid == src->blockid) {
                        if (InvSlot_transfer(&dest->hotbar[i], src)) {
                                return 1;
                        }
                }
        }

        // Look for exact match in inventory
        for (int i = 0; i < INVENTORY_SIZE; i ++) {
                if(dest->slots[i].blockid == src->blockid) {
                        if (InvSlot_transfer(&dest->slots[i], src)) {
                                return 1;
                        }
                }
        }
        // Look for empty slot in inventory
        for (int i = 0; i < HOTBAR_SIZE; i ++) {
                if(dest->hotbar[i].blockid == 0) {
                        if (InvSlot_transfer(&dest->hotbar[i], src)) {
                                return 1;
                        }
                }
        }

        // Look for exact match in inventory
        for (int i = 0; i < INVENTORY_SIZE; i ++) {
                if(dest->slots[i].blockid == 0) {
                        if (InvSlot_transfer(&dest->slots[i], src)) {
                                return 1;
                        }
                }
        }

        return 0;
}

/* Inventory_save
 * Saves inventory contents to the specified file. Only intended for use inside
 * of Player_save.
 */
static void Inventory_save (FILE *file, Inventory *inventory) {
        InvSlot_save(file, &inventory->offhand);
        
        for (int index = 0; index < HOTBAR_SIZE; index ++) {
                InvSlot_save(file, &inventory->hotbar[index]);
        }
        
        for (int index = 0; index < INVENTORY_SIZE; index ++) {
                InvSlot_save(file, &inventory->slots[index]);
        }
        
        for (int index = 0; index < ARMOR_SIZE; index ++) {
                InvSlot_save(file, &inventory->armor[index]);
        }

        fprintf(file, "%i\n", inventory->hotbarSelect);
}

/* Inventory_load
 * Loads inventory contents from the specified file. Only intended for use
 * inside of Player_load.
 */
static void Inventory_load (FILE *file, Inventory *inventory) {
        InvSlot_load(file, &inventory->offhand);
        
        for (int index = 0; index < HOTBAR_SIZE; index ++) {
                InvSlot_load(file, &inventory->hotbar[index]);
        }
        
        for (int index = 0; index < INVENTORY_SIZE; index ++) {
                InvSlot_load(file, &inventory->slots[index]);
        }
        
        for (int index = 0; index < ARMOR_SIZE; index ++) {
                InvSlot_load(file, &inventory->armor[index]);
        }

        fscanf(file, "%i\n", &inventory->hotbarSelect);
}

/* InvSlot_save
 * Saves inventory slot to the specified file. Only intended for use inside of
 * Inventory_save.
 */
static void InvSlot_save (FILE *file, InvSlot *invSlot) {
        fprintf (file, "%hhu %hhu %hu\n",
                invSlot->blockid,
                invSlot->amount,
                invSlot->durability);
}

/* InvSlot_load
 * Loads inventory slot description into the specified slot. Only indented for
 * use inside of Inventory_load.
 */
static void InvSlot_load (FILE *file, InvSlot *invSlot) {
        fscanf (file, "%hhu %hhu %hu\n",
                &invSlot->blockid,
                &invSlot->amount,
                &invSlot->durability);
}

/* InvSlot_transfer
 * Transfers items from src int dest. This will transfer as many items as it
 * can. Returns true if all items were transferred, false if some items weren't.
 *
 * This function will overwrite data about the item, so only call this function
 * if you know that dest and src contain the same thing (or dest is empty).
 */
int InvSlot_transfer (InvSlot *dest, InvSlot *src) {
        int want = 64 - dest->amount;
        if (want > src->amount) {
                want = src->amount;
        }

        dest->amount += want;
        src->amount  -= want;

        dest->blockid    = src->blockid;
        dest->durability = src->durability;

        return src->amount == 0;
}

/* InvSlow_swap
 * Swaps the data in two inventory slots. This is used for inventory management.
 */
void InvSlot_swap (InvSlot *left, InvSlot *right) {
        InvSlot temp = *left;
        *left  = *right;
        *right = temp;
}
