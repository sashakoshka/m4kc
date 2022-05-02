#include <time.h>
#include "menus.h"
#include "data.h"
#include "blocks.h"
#include "gameloop.h"

static int menu_optionsMain (SDL_Renderer *, Inputs *);

/* === GAME STATES === */

/* state_title
 * Presents a title screen with basic options. Is capable of changing the game
 * state.
 */
int state_title (SDL_Renderer *renderer, Inputs *inputs, int *gameState) {
        inputs->mouse.x /= BUFFER_SCALE;
        inputs->mouse.y /= BUFFER_SCALE;

        dirtBg(renderer);
        white(renderer);
        drawBig (
                renderer,
                "M4KC",
                BUFFER_HALF_W,
                16
        );

        #ifdef small
        shadowStr(renderer, "version 0.7", 1, BUFFER_H - 9);
        #else
        shadowStr(renderer, "version 0.7 (dev build)", 1, BUFFER_H - 9);
        #endif

        if (button(renderer, "Singleplayer",
                BUFFER_HALF_W - 64, 42, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                if (data_refreshWorldList()) {
                        gameLoop_error("Cannot refresh world list");
                } else {
                        *gameState = STATE_SELECT_WORLD;
                }
        }

        if (button(renderer, "Options",
                BUFFER_HALF_W - 64, 64, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                *gameState = STATE_OPTIONS;
        }

        if (button(renderer, "Quit Game",
                BUFFER_HALF_W - 64, 86, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                return 1;
        }

        return 0;
}

void state_selectWorld (
        SDL_Renderer *renderer,
        Inputs *inputs,
        int *gameState
) {
        SDL_Rect listBackground;
        listBackground.x = 0;
        listBackground.y = 0;
        listBackground.w = BUFFER_W;
        listBackground.h = BUFFER_H - 28;
        
        inputs->mouse.x /= BUFFER_SCALE;
        inputs->mouse.y /= BUFFER_SCALE;

        dirtBg(renderer);
        tblack(renderer);
        SDL_RenderFillRect (renderer, &listBackground);
        SDL_RenderDrawLine (renderer,
                0,        BUFFER_H - 29,
                BUFFER_W, BUFFER_H - 29);

        white(renderer);
        int y = 6;
        data_WorldListItem *item = data_worldList;
        while (item != NULL) {
                drawWorldListItem (renderer, item,
                        BUFFER_HALF_W - 64, y,
                        inputs->mouse.x, inputs->mouse.y);
                y += 22;
                item = item->next;
        }

        if (y == 6) {
                shadowCenterStr (renderer, "No worlds",
                        BUFFER_HALF_W, BUFFER_HALF_H - 15);
        }

        if (button(renderer, "Cancel",
                BUFFER_HALF_W - 64, BUFFER_H - 22, 61,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                *gameState = STATE_TITLE;
        }

        if (button(renderer, "New",
                BUFFER_HALF_W + 3, BUFFER_H - 22, 61,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                *gameState = STATE_NEW_WORLD;
        }
}

const char *terrainNames[16] = {
        "Classic Terrain",
        "Natural Terrain",
        "Flat Stone",
        "Flat Grass",
        "Water World"
};

const char *dayNightModes[16] = {
        "Day and Night",
        "Always Day",
        "Always Night",
};

/* state_newWorld
 * Shows a menu with editable parameters for creating a new world. Capable of
 * editing world prarameters and changing the game state.
 */
void state_newWorld (
        SDL_Renderer *renderer,
        Inputs *inputs,
        int *gameState,
        int *type,
        int *dayNightMode,
        int *seed,
        char *path
) {
        static int badName    = 0;
        static int whichInput = 0;

        static int typeSelect     = 1;
        static int dayNightSelect = 0;
        
        static char seedBuffer[16];
        static InputBuffer seedInput = {
                .buffer = seedBuffer,
                .len    = 16,
                .cursor = 0
        };
        
        static char nameBuffer[16];
        static InputBuffer nameInput = {
                .buffer = nameBuffer,
                .len    = 16,
                .cursor = 0
        };
                
        inputs->mouse.x /= BUFFER_SCALE;
        inputs->mouse.y /= BUFFER_SCALE;

        dirtBg(renderer);
        
        if (whichInput == 0) {manageInputBuffer(&nameInput, inputs); }
        if (input(renderer, "Name", nameInput.buffer,
                BUFFER_HALF_W - 64, 8, 128,
                inputs->mouse.x, inputs->mouse.y, whichInput == 0) &&
                inputs->mouse.left
        ) {
                whichInput = 0;
        }

        if (badName) {
                SDL_SetRenderDrawColor(renderer, 255, 128, 128, 255);
                drawChar(renderer, '!', BUFFER_HALF_W + 70, 12);
        }
        
        if (whichInput == 1) {manageInputBuffer(&seedInput, inputs); }
        if (input(renderer, "Seed", seedInput.buffer,
                BUFFER_HALF_W - 64, 30, 128,
                inputs->mouse.x, inputs->mouse.y, whichInput == 1) &&
                inputs->mouse.left
        ) {
                whichInput = 1;
        }
        
        if (button(renderer, terrainNames[typeSelect],
                BUFFER_HALF_W - 64, 52, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                typeSelect = (typeSelect + 1) % 5;
        }


        if (button(renderer, dayNightModes[dayNightSelect],
                BUFFER_HALF_W - 64, 74, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                dayNightSelect = (dayNightSelect + 1) % 3;
        }

        if (button(renderer, "Cancel",
                BUFFER_HALF_W - 64, 96, 61,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                *gameState = STATE_SELECT_WORLD;
        }

        if (button(renderer, "Generate",
                BUFFER_HALF_W + 3, 96, 61,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                // Reject empty names and names with slashes
                if (nameInput.buffer[0] == 0) {
                        goto cantMakeWorld;
                }
                for (int index = 0; nameInput.buffer[index]; index ++) {
                        if (nameInput.buffer[index] == '/') {
                                goto cantMakeWorld;
                        }
                }

                if (data_getWorldPath(path, nameInput.buffer)) {
                        goto cantMakeWorld;
                }

                if (data_directoryExists(path)) {
                        goto cantMakeWorld;
                }
                
                *type         = typeSelect;
                *dayNightMode = dayNightSelect;
                *gameState    = STATE_LOADING;

                // Get numeric seed
                *seed = 0;
                for (int index = 0; seedInput.buffer[index]; index ++) {
                        *seed *= 10;
                        *seed += seedInput.buffer[index] - '0';
                }

                // "Randomize" seed if it was not set
                if (*seed == 0) {
                        *seed = time(0) % 999999999999999;
                }

                // Secret world for testing nonsense. Type "dev"
                if (*seed == 5800) {
                        *type = -1;
                }
                
                whichInput = 0;
                
                seedInput.buffer[0] = 0;
                seedInput.cursor    = 0;
                
                nameInput.buffer[0] = 0;
                nameInput.cursor    = 0;
                badName             = 0;
        }

        return;

        cantMakeWorld:
        nameInput.buffer[0] = 0;
        nameInput.cursor    = 0;
        badName             = 1;
}

/* state_loading
 * Shows a loading screen and progressively loads in chunks. Returns 1 when
 * finished.
 */
int state_loading (
        SDL_Renderer *renderer,
        World *world,
        unsigned int seed,
        Coords center
) {
        IntCoords chunkLoadCoords;
        static int chunkLoadNum = 0;
        
        if (chunkLoadNum < CHUNKARR_SIZE) {
                chunkLoadCoords.x =
                        ((chunkLoadNum % CHUNKARR_DIAM) -
                        CHUNKARR_RAD) * 64;
                chunkLoadCoords.y =
                        (((chunkLoadNum / CHUNKARR_DIAM) % CHUNKARR_DIAM) - 
                        CHUNKARR_RAD) * 64;
                chunkLoadCoords.z =
                        ((chunkLoadNum / (CHUNKARR_DIAM * CHUNKARR_DIAM)) -
                        CHUNKARR_RAD) * 64;
                genChunk (
                        world, seed,
                        chunkLoadCoords.x,
                        chunkLoadCoords.y,
                        chunkLoadCoords.z, world->type, 1,
                        center
                );
                loadScreen (
                        renderer,
                        "Generating world...",
                        chunkLoadNum, CHUNKARR_SIZE
                );
                chunkLoadNum++;
                return 0;
        } else {
                chunkLoadNum = 0;
                return 1;
        }
}

/* state_options
 * Shows an options screen. Capable of changing settings and the game state.
 */
void state_options (SDL_Renderer *renderer, Inputs *inputs, int *gameState) {
        inputs->mouse.x /= BUFFER_SCALE;
        inputs->mouse.y /= BUFFER_SCALE;

        dirtBg(renderer);

        if (menu_optionsMain (renderer, inputs)) {
                *gameState = 0;
        }
}

/* state_egg
 * This lacks description. Capable of changing the game state.
 */
void state_egg (SDL_Renderer *renderer, Inputs *inputs, int *gameState) {
        inputs->mouse.x /= BUFFER_SCALE;
        inputs->mouse.y /= BUFFER_SCALE;

        dirtBg(renderer);
        white(renderer);
        centerStr (
                renderer,
                "Go away, this is my house.",
                BUFFER_HALF_W,
                BUFFER_HALF_H - 16
        );
        if (button(renderer, "Ok",
                BUFFER_HALF_W - 64, BUFFER_HALF_H, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                *gameState = STATE_TITLE;
        }
}

/* state_err
 * Shows an error message on screen. Returns 1 when the "Ok" button is pressed.
 */
int state_err (SDL_Renderer *renderer, Inputs *inputs, char *message) {
        inputs->mouse.x /= BUFFER_SCALE;
        inputs->mouse.y /= BUFFER_SCALE;

        dirtBg(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 128, 128, 255);
        centerStr (
                renderer,
                "Error:",
                BUFFER_HALF_W,
                BUFFER_HALF_H - 20
        );
        white(renderer);
        centerStr (
                renderer,
                message,
                BUFFER_HALF_W,
                BUFFER_HALF_H - 4
        );
        if (button(renderer, "Ok",
                BUFFER_HALF_W - 64, BUFFER_HALF_H + 16, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                return 1;
        }
        return 0;
}

/* === INGAME POPUPS === */

/* popup_hud
 * Draws the heads up display, including the hotbar, offhand, crosshair, health,
 * hunger, chat history, and the debug menu if activated.
 */
void popup_hud (
        SDL_Renderer *renderer, Inputs *inputs, World *world,
        int *debugOn, u_int32_t *fps_now,
        Player *player
) {
        int i;

        static SDL_Rect hotbarRect;
        hotbarRect.x = BUFFER_HALF_W - 77;
        hotbarRect.y = BUFFER_H - 18;
        hotbarRect.w = 154;
        hotbarRect.h = 18;

        static SDL_Rect hotbarSelectRect;
        hotbarSelectRect.x = 0;
        hotbarSelectRect.y = hotbarRect.y;
        hotbarSelectRect.w = 18;
        hotbarSelectRect.h = 18;

        static SDL_Rect offhandRect;
        offhandRect.x = 0;
        offhandRect.y = BUFFER_H - 18;
        offhandRect.w = 18;
        offhandRect.h = 18;

        // Debug screen
        if (*debugOn) {
                static char debugText [][32] = {
                        "M4KC 0.7",
                        "Seed: ",
                        "X: ",
                        "Y: ",
                        "Z: ",
                        "FPS: ",
                        "ChunkX: ",
                        "ChunkY: ",
                        "ChunkZ: "
                };

                // Seed
                strnum(debugText[1], 6, world->seed);

                // Coordinates
                strnum(debugText[2], 3, (int)player->pos.x - 64);
                strnum(debugText[3], 3, (int)player->pos.y - 64);
                strnum(debugText[4], 3, (int)player->pos.z - 64);

                // FPS
                strnum(debugText[5], 5, *fps_now);

                // Chunk coordinates
                strnum(debugText[6], 8, ((int)player->pos.x - 64) >> 6);
                strnum(debugText[7], 8, ((int)player->pos.y - 64) >> 6);
                strnum(debugText[8], 8, ((int)player->pos.z - 64) >> 6);

                // Text
                for (i = 0; i < 9; i++) {
                        drawBGStr(renderer, debugText[i], 0, i * 9);
                }

                // Chunk monitor
                #ifndef small
                #define CHUNKMONW   10
                #define CHUNKMONCOL 9

                SDL_Rect chunkMonitorRect = { 
                        .x = 0,
                        .y = 1 - CHUNKMONW,
                        .w = CHUNKMONW,
                        .h = CHUNKMONW
                };
                for (i = 0; i < CHUNKARR_SIZE; i++) {
                        if (i % CHUNKMONCOL == 0) {
                                chunkMonitorRect.x = BUFFER_W - (
                                        (CHUNKMONW * (CHUNKMONCOL - 1)) + 2);
                                chunkMonitorRect.y += CHUNKMONW - 1;
                        } else {
                                chunkMonitorRect.x += CHUNKMONW - 1;
                        }

                        int stamp = world->chunk[i].loaded;
                        SDL_SetRenderDrawColor (
                                renderer,
                                (stamp & 0b000011) * 64,
                                (stamp & 0b001100) * 16,
                                (stamp & 0b110000) * 4,
                                0xFF
                        );
                        SDL_RenderFillRect(renderer, &chunkMonitorRect);
                        white(renderer);
                        SDL_RenderDrawRect(renderer, &chunkMonitorRect);
                }

                #undef CHUNKMONW
                #undef CHUNKMONCOL
                #endif
        }
        
        // Hotbar
        tblack(renderer);
        SDL_RenderFillRect(renderer, &hotbarRect);

        hotbarSelectRect.x =
        BUFFER_HALF_W - 77 + player->inventory.hotbarSelect * 17;
        white(renderer);
        SDL_RenderDrawRect(renderer, &hotbarSelectRect);

        for (i = 0; i < 9; i++) {
                drawSlot (
                        renderer,
                        &player->inventory.hotbar[i],
                        BUFFER_HALF_W - 76 + i * 17,
                        BUFFER_H - 17,
                        inputs->mouse.x,
                        inputs->mouse.y
                );
        }

        // Offhand
        if (player->inventory.offhand.blockid != 0) {
                tblack(renderer);
                SDL_RenderDrawRect(renderer, &offhandRect);
                drawSlot (
                        renderer,
                        &player->inventory.offhand,
                        1,
                        BUFFER_H - 17,
                        inputs->mouse.x,
                        inputs->mouse.y
                );
        }

        // Chat
        int chatDrawIndex = chatHistoryIndex;
        for (i = 0; i < 11; i++) {
                chatDrawIndex = nmod(chatDrawIndex - 1, 11);
                if (chatHistoryFade[chatDrawIndex] > 0) {
                        chatHistoryFade[chatDrawIndex]--;
                        drawBGStr(
                                renderer, chatHistory[chatDrawIndex],
                                0, BUFFER_H - 32 - i * 9
                        );
                }
        }
}

/* manageInvSlot
 * Draws and performs the input logic of a single inventory slot. Capable of
 * changing which slot is currently being dragged.
 */
void manageInvSlot (
        SDL_Renderer *renderer,
        Inputs  *inputs,
        int     x,
        int     y,
        InvSlot *current,
        InvSlot *selected,
        int     *dragging
) {
        if (drawSlot (
                renderer,
                current,
                x, y,
                inputs->mouse.x,
                inputs->mouse.y
        ) && inputs->mouse.left) {
                inputs->mouse.left = 0;
                if (*dragging) {
                        // Place down item
                        if (current->blockid == 0) {
                                *current  = *selected;
                                *selected = (const InvSlot) { 0 };
                                *dragging = 0;
                        } else if (current->blockid == selected->blockid) {
                                InvSlot_transfer(current, selected);
                        } else {
                                InvSlot_swap(current, selected);
                        }
                        
                } else if (current->blockid != 0) {
                        // Pick up item
                        *selected = *current;
                        *current  = (const InvSlot) { 0 };
                        *dragging = 1;
                }
        }
}

/* popup_inventory
 * Allows the user to manage their inventory, rearranging the items inside of
 * it. Capable of closing itself.
 */
void popup_inventory (
        SDL_Renderer *renderer,
        Inputs *inputs,
        Player *player,
        int *gamePopup
) {     
        SDL_Rect inventoryRect;
        inventoryRect.x = BUFFER_HALF_W - 77;
        inventoryRect.y = (BUFFER_H - 18) / 2 - 26;
        inventoryRect.w = 154;
        inventoryRect.h = 52;

        SDL_Rect hotbarRect;
        hotbarRect.x = BUFFER_HALF_W - 77;
        hotbarRect.y = BUFFER_H - 18;
        hotbarRect.w = 154;
        hotbarRect.h = 18;

        SDL_Rect offhandRect;
        offhandRect.x = 0;
        offhandRect.y = BUFFER_H - 18;
        offhandRect.w = 18;
        offhandRect.h = 18;

        static InvSlot selected = { 0 };
        static int dragging = 0;

        // Inventory background
        tblack(renderer);
        SDL_RenderFillRect(renderer, &inventoryRect);
        SDL_RenderFillRect(renderer, &hotbarRect);
        SDL_RenderFillRect(renderer, &offhandRect);

        // Hotbar items
        for (int i = 0; i < HOTBAR_SIZE; i++) {
                manageInvSlot (
                        renderer, inputs,
                        BUFFER_HALF_W - 76 + i * 17,
                        BUFFER_H - 17,
                        &player->inventory.hotbar[i],
                        &selected,
                        &dragging
                );
        }

        // Inventory items
        for (int i = 0; i < INVENTORY_SIZE; i++) {
                manageInvSlot (
                        renderer, inputs,
                        BUFFER_HALF_W - 76 + (i % HOTBAR_SIZE) * 17,
                        inventoryRect.y + 1 + (i / HOTBAR_SIZE) * 17,
                        &player->inventory.slots[i],
                        &selected,
                        &dragging
                );
        }

        // Offhand
        manageInvSlot (
                renderer, inputs,
                1,
                BUFFER_H - 17,
                &player->inventory.offhand,
                &selected,
                &dragging
        );

        if (dragging) {
                drawSlot (
                        renderer,
                        &selected,
                        inputs->mouse.x - 8,
                        inputs->mouse.y - 8,
                        0, 0
                );
        }
        
        // Exit inventory
        if (inputs->keyboard.e) {
                inputs->keyboard.e = 0;
                *gamePopup = POPUP_HUD;
        }
}

/* popup_chat
 * Allows the user to type in chat, and view farther back in the message
 * history. Capable of closing itself.
 */
void popup_chat (SDL_Renderer *renderer, Inputs *inputs, u_int64_t gameTime) {
        static char buffer[64] = { 0 };
        static InputBuffer chatBox = {
                .buffer = buffer,
                .len    = 64,
                .cursor = 0
        };
        
        static SDL_Rect chatBoxRect = {0, 0, 0, 9};
        chatBoxRect.y = BUFFER_H - 9;
        chatBoxRect.w = BUFFER_W;

        int chatDrawIndex = chatHistoryIndex;
        for (int i = 0; i < 11; i++) {
                chatDrawIndex = nmod(chatDrawIndex - 1, 11);
                drawBGStr(
                        renderer, chatHistory[chatDrawIndex],
                        0, BUFFER_H - 32 - i * 9
                );
        }

        // Get keyboard input
        if (manageInputBuffer(&chatBox, inputs)) {
                // 63: max chat box length
                // 7:  max username length
                // 2:  ": " chars
                // 1:  null
                static char chatNameConcat[63 + 7 + 2 + 1];
                snprintf (chatNameConcat,  63 + 7 + 2, "%s: %s",
                        data_options.username.buffer, chatBox.buffer);
                
                // Add input to chat
                chatAdd(chatNameConcat);
                // Clear input box
                chatBox.cursor = 0;
                chatBox.buffer[0] = 0;
        }

        // Chat input box
        // If char limit is reached, give some visual
        // feedback.
        if (chatBox.cursor == 63) {
                SDL_SetRenderDrawColor(renderer, 128, 0, 0, 128);
        } else {
                tblack(renderer);
        }
        
        SDL_RenderFillRect(renderer, &chatBoxRect);

        white(renderer);
        drawChar (
                renderer,
                95 + 32 * ((gameTime >> 6) % 2),
                drawStr (
                        renderer, chatBox.buffer,
                        0, BUFFER_H - 8
                ),
                BUFFER_H - 8
        );
}

/* popup_pause
 * Displays a pause menu. Capable of activating submenus or changing the game
 * state.
 */
void popup_pause (
        SDL_Renderer *renderer, Inputs *inputs,
        int *gamePopup, int *gameState
) {
        if (button(renderer, "Back to Game",
                BUFFER_HALF_W - 64, 20, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                *gamePopup = POPUP_HUD;
        }

        if (button(renderer, "Options...",
                BUFFER_HALF_W - 64, 42, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                *gamePopup = POPUP_OPTIONS;
        }

        if (button(renderer, "Quit to Title",
                BUFFER_HALF_W - 64, 64, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                *gameState = STATE_TITLE;
        }
}

/* popup_options
 * Shows an options screen. Capable of changing settings and closing itself.
 */
void popup_options (SDL_Renderer *renderer, Inputs *inputs, int *gamePopup) {
        if (menu_optionsMain(renderer, inputs)) {
                *gamePopup = 1;
        }
}

#ifndef small
/* popup_debugTools
 * Shows a menu listing advanced debug tools. These are only included in debug
 * builds and are not included in compressed executables.
 */
void popup_debugTools (SDL_Renderer *renderer, Inputs *inputs, int *gamePopup) {
        if (button(renderer, "Chunk Peek",
                BUFFER_HALF_W - 64, 20, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                *gamePopup = POPUP_CHUNK_PEEK;
        }

        if (button(renderer, "Done",
                BUFFER_HALF_W - 64, 42, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                *gamePopup = POPUP_HUD;
        }
}

/* popup_chunkPeek
 * Shows a 3D map of the current chunk, with the ability to view a cross-section
 * of it. This feature is only included in debug builds.
 */
void popup_chunkPeek (
        SDL_Renderer *renderer, Inputs *inputs, World *world,
        int *gamePopup,
        Player *player
) {
        static int chunkPeekRYMax = 0;

        int chunkPeekRX,
            chunkPeekRY,
            chunkPeekRZ,
            chunkPeekColor;
        
        Chunk *debugChunk;
        char chunkPeekText[][32] = {
                "coordHash: ",
                "loaded: "
        };

        debugChunk = chunkLookup (
                world,
                (int)player->pos.x - 64,
                (int)player->pos.y - 64,
                (int)player->pos.z - 64
        );

        white(renderer);
        if (debugChunk != NULL) {
                // There is a chunk to display info about. Process
                // strings.
                strnum(chunkPeekText[0], 11, debugChunk -> coordHash);
                strnum(chunkPeekText[1], 8,  debugChunk -> loaded);
                // Draw the strings
                for (int i = 0; i < 2; i++) {
                        drawStr(renderer, chunkPeekText[i], 0, i << 3);
                }

                // Scroll wheel for changing chunk map xray
                if (inputs->mouse.wheel != 0) {
                        chunkPeekRYMax -= inputs->mouse.wheel;
                        chunkPeekRYMax = nmod(chunkPeekRYMax, 64);
                        inputs->mouse.wheel = 0;
                }

                // Mouse for changing chunk map xray
                if (
                        inputs->mouse.x > 128 &&
                        inputs->mouse.y < 64  &&
                        inputs->mouse.left
                ) chunkPeekRYMax = inputs->mouse.y;

                // Up/Down buttons for changing chunk map xray
                if (button(renderer, "UP",
                        4, 56, 64,
                        inputs->mouse.x, inputs->mouse.y)
                        && inputs->mouse.left
                ) {
                        chunkPeekRYMax = nmod(chunkPeekRYMax - 1, 64);
                }

                if (button(renderer, "DOWN",
                        4, 78, 64,
                        inputs->mouse.x, inputs->mouse.y)
                        && inputs->mouse.left
                ) {
                        chunkPeekRYMax = nmod(chunkPeekRYMax + 1, 64);
                }

                // Draw chunk map
                white(renderer);
                SDL_RenderDrawLine (
                        renderer,
                        128, chunkPeekRYMax,
                        191, chunkPeekRYMax
                );
                
                for (
                        chunkPeekRY = 64;
                        chunkPeekRY >= chunkPeekRYMax;
                        chunkPeekRY--
                ) for (
                        chunkPeekRX = 0;
                        chunkPeekRX < 64;
                        chunkPeekRX++
                ) for (
                        chunkPeekRZ = 0;
                        chunkPeekRZ < 63;
                        chunkPeekRZ++
                ) {
                        Block currentBlock = debugChunk->blocks [
                                chunkPeekRX +
                                (chunkPeekRY << 6) +
                                (chunkPeekRZ << 12)];
                        
                        chunkPeekColor = textures [
                                currentBlock * 256 * 3 + 6 * 16];

                        if (chunkPeekColor) {
                                int alpha = 255;
                                
                                if (currentBlock == BLOCK_WATER) {
                                        alpha = 64;
                                }
                        
                                SDL_SetRenderDrawColor (
                                        renderer,
                                        (chunkPeekColor >> 16 & 0xFF),
                                        (chunkPeekColor >> 8 & 0xFF),
                                        (chunkPeekColor & 0xFF),
                                        alpha);
                                
                                SDL_RenderDrawPoint (
                                        renderer,
                                        chunkPeekRX + 128,
                                        chunkPeekRY + chunkPeekRZ);
                                        
                                // A little shadow for depth
                                SDL_SetRenderDrawColor (
                                        renderer,
                                        0, 0, 0, 64);
                                
                                SDL_RenderDrawPoint (
                                        renderer,
                                        chunkPeekRX + 128,
                                        chunkPeekRY + chunkPeekRZ + 1);
                        }
                }
        } else {
                drawStr(renderer, "NULL chunk!", 0, 0); 
        }

        if (button(renderer, "Done",
                4, 100, 64,
                inputs->mouse.x, inputs->mouse.y)
                && inputs->mouse.left
        ) {
                *gamePopup = POPUP_ADVANCED_DEBUG;
        }
}
#endif

/* menu_optionsMain
 * This function presents the options menu. It's purpose is to be included in
 * other functions that draw a suitable background and then call this one.
 * Returns 1 when the user pressed the "Done" button. Capable of changing
 * settings.
 */
static int menu_optionsMain (SDL_Renderer *renderer, Inputs *inputs) {
        static int page = 0;

        switch (page) {
        case 0:
                manageInputBuffer(&data_options.username, inputs);
                input (renderer, "Username", data_options.username.buffer,
                        BUFFER_HALF_W - 64, 20, 128,
                        inputs->mouse.x, inputs->mouse.y, 1);

                static char *trapMouseTexts[] = {
                        "Capture Mouse: OFF",
                        "Capture Mouse: ON"
                };
                if (button(renderer, trapMouseTexts[data_options.trapMouse],
                        BUFFER_HALF_W - 64, 42, 128,
                        inputs->mouse.x, inputs->mouse.y) &&
                        inputs->mouse.left
                ) {
                        data_options.trapMouse = !data_options.trapMouse;
                }
                break;
        case 1:
                ;static char drawDistanceText[] = "Draw distance: 20\0";
                if (button(renderer, drawDistanceText,
                        BUFFER_HALF_W - 64, 20, 128,
                        inputs->mouse.x, inputs->mouse.y) &&
                        inputs->mouse.left
                ) {
                        switch (data_options.drawDistance) {
                        case 20:
                                data_options.drawDistance = 32;
                                break;
                        case 32:
                                data_options.drawDistance = 64;
                                break;
                        case 64:
                                data_options.drawDistance = 96;
                                break;
                        case 96:
                                data_options.drawDistance = 128;
                                break;
                        default:
                                data_options.drawDistance = 20;
                                break;
                        }
                        strnum(drawDistanceText, 15, data_options.drawDistance);
                }

                static char *fovTexts[] = {
                        "FOV: Low",
                        "FOV: Medium",
                        "FOV: High",
                };
                char *fovText = NULL;
                switch ((int)data_options.fov) {
                        case 60:  fovText = fovTexts[2]; break;
                        case 90:  fovText = fovTexts[1]; break;
                        case 140: fovText = fovTexts[0]; break;
                }
                
                if (button(renderer, fovText,
                        BUFFER_HALF_W - 64, 42, 128,
                        inputs->mouse.x, inputs->mouse.y) &&
                        inputs->mouse.left
                ) {
                        switch ((int)data_options.fov) {
                                case 60:  data_options.fov = 140; break;
                                case 90:  data_options.fov = 60;  break;
                                default:  data_options.fov = 90;  break;
                        }
                        
                }

                static char *fogTexts[] = {
                        "Fog: Gradual",
                        "Fog: Sharp"
                };
                if (button(renderer, fogTexts[data_options.fogType],
                        BUFFER_HALF_W - 64, 64, 128,
                        inputs->mouse.x, inputs->mouse.y) &&
                        inputs->mouse.left
                ) {
                        data_options.fogType = !data_options.fogType;
                }
                
                break;
        }

        if (button(renderer, "<",
                BUFFER_HALF_W - 86, 20, 16,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                page --;
                page = nmod(page, 2);
        }

        if (button(renderer, ">",
                BUFFER_HALF_W + 70, 20, 16,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                page ++;
                page = nmod(page, 2);
        }

        if (button(renderer, "Done",
                BUFFER_HALF_W - 64, 86, 128,
                inputs->mouse.x, inputs->mouse.y) &&
                inputs->mouse.left
        ) {
                page = 0;
                return 1;
        }

        return 0;
}
