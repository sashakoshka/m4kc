#include "player.h"

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
                if(dest->hotbar[i].blockid == src->blockid) {
                        if (InvSlot_transfer(&dest->hotbar[i], src)) {
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
                if(dest->hotbar[i].blockid == 0) {
                        if (InvSlot_transfer(&dest->hotbar[i], src)) {
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
