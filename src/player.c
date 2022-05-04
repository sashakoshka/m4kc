#include <stdlib.h>
#include <stdio.h>
#include "player.h"

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
                "%hu\n",
                0,
                player->pos.x, player->pos.y, player->pos.z,
                player->hRot, player->vRot,
                player->health, player->hunger, player->breath,
                player->xp);

        // TODO: save inventory
        
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
                "%a %a %a "
                "%a %a "
                "%hhu %hhu %hhu "
                "%hu",
                &player->pos.x, &player->pos.y, &player->pos.z,
                &player->hRot, &player->vRot,
                &player->health, &player->hunger, &player->breath,
                &player->xp
                );

        // TODO: load inventory
        
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
