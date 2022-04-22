#include "menus.h"

int menu_optionsMain (SDL_Renderer *, Inputs *, int *, int *);

/* === GAME STATES === */

int state_title (
        SDL_Renderer *renderer, Inputs *inputs,
        int *gameState, int *init
) {
        inputs->mouse_X /= BUFFER_SCALE;
        inputs->mouse_Y /= BUFFER_SCALE;

        dirtBg(renderer);
        white(renderer);
        drawBig (
                renderer,
                "M4KC",
                BUFFER_HALF_W,
                16
        );

        shadowStr(renderer, "version 0.7 (pre-release)", 1, BUFFER_H - 9);

        if (button(renderer, "Singleplayer",
                BUFFER_HALF_W - 64, 42, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                *gameState = 4;
                *init = 1;
        }

        if (button(renderer, "Options",
                BUFFER_HALF_W - 64, 64, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                *gameState = 8;
        }

        if (button(renderer, "Quit Game",
                BUFFER_HALF_W - 64, 86, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                return 1;
        }

        return 0;
}

int state_loading (
        SDL_Renderer *renderer,
        World *world,
        unsigned int seed,
        Coords center
) {
        IntCoords chunkLoadCoords;
        static int chunkLoadNum = 0;
        
        if(chunkLoadNum < CHUNKARR_SIZE) {
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
                        chunkLoadCoords.z, 1, 1,
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

void state_options (
        SDL_Renderer *renderer, Inputs *inputs,
        int *gameState, int *drawDistance, int *trapMouse
) {
        inputs->mouse_X /= BUFFER_SCALE;
        inputs->mouse_Y /= BUFFER_SCALE;

        dirtBg(renderer);

        if(menu_optionsMain(renderer, inputs, drawDistance, trapMouse)) {
                *gameState = 0;
        }
}

void state_egg (SDL_Renderer *renderer, Inputs *inputs, int *gameState) {
        inputs->mouse_X /= BUFFER_SCALE;
        inputs->mouse_Y /= BUFFER_SCALE;

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
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                *gameState = 0;
        }
}

/* === INGAME POPUPS === */

void popup_hud (
        SDL_Renderer *renderer, Inputs *inputs, World *world,
        int *debugOn, u_int32_t *fps_now,
        Player *player
) {
        static SDL_Rect hotbarRect;
        static SDL_Rect hotbarSelectRect;

        int i;

        hotbarRect.x = BUFFER_HALF_W - 77;
        hotbarRect.y = BUFFER_H - 18;
        hotbarRect.w = 154;
        hotbarRect.h = 18;

        hotbarSelectRect.y = hotbarRect.y;
        hotbarSelectRect.w = 18;
        hotbarSelectRect.h = 18;

        // Debug screen
        if (*debugOn) {
                static char debugText [][16] = {
                        "M4KC 0.7",
                        "X: ",
                        "Y: ",
                        "Z: ",
                        "FPS: ",
                        "ChunkX: ",
                        "ChunkY: ",
                        "ChunkZ: ",
                };

                // Coordinates
                strnum(debugText[1], 3, (int)player->pos.x - 64);
                strnum(debugText[2], 3, (int)player->pos.y - 64);
                strnum(debugText[3], 3, (int)player->pos.z - 64);

                // FPS
                strnum(debugText[4], 5, *fps_now);

                // Chunk coordinates
                strnum(debugText[5], 8, ((int)player->pos.x - 64) >> 6);
                strnum(debugText[6], 8, ((int)player->pos.y - 64) >> 6);
                strnum(debugText[7], 8, ((int)player->pos.z - 64) >> 6);

                // Text
                for (i = 0; i < 8; i++) { drawBGStr(renderer, debugText[i], 0, i * 9); }

                // Chunk monitor
                #ifndef small
                #define CHUNKMONW   10
                #define CHUNKMONCOL 9

                SDL_Rect chunkMonitorRect = {0, 1 - CHUNKMONW, CHUNKMONW, CHUNKMONW};
                for (i = 0; i < CHUNKARR_SIZE; i++) {
                        if (i % CHUNKMONCOL == 0) {
                                chunkMonitorRect.x = BUFFER_W - ((CHUNKMONW * (CHUNKMONCOL - 1)) + 2);
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
                        inputs->mouse_X,
                        inputs->mouse_Y
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

void popup_inventory (
        SDL_Renderer *renderer,
        Inputs *inputs,
        Player *player,
        int *gamePopup
) {     
        static SDL_Rect inventoryRect;
        inventoryRect.x = BUFFER_HALF_W - 77;
        inventoryRect.y = (BUFFER_H - 18) / 2 - 26;
        inventoryRect.w = 154;
        inventoryRect.h = 52;

        static SDL_Rect hotbarRect;
        hotbarRect.x = BUFFER_HALF_W - 77;
        hotbarRect.y = BUFFER_H - 18;
        hotbarRect.w = 154;
        hotbarRect.h = 18;

        static InvSlot selected = { 0 };
        static int dragging = 0;

        // Inventory background
        tblack(renderer);
        SDL_RenderFillRect(renderer, &inventoryRect);
        SDL_RenderFillRect(renderer, &hotbarRect);

        // Hotbar items
        for (int i = 0; i < HOTBAR_SIZE; i++) {
                InvSlot *current = &player->inventory.hotbar[i];
        
                if (drawSlot (
                        renderer,
                        current,
                        BUFFER_HALF_W - 76 + i * 17,
                        BUFFER_H - 17,
                        inputs->mouse_X,
                        inputs->mouse_Y
                ) && inputs->mouse_Left) {
                        inputs->mouse_Left = 0;
                        if (dragging) {
                                // Place down item
                                
                                
                                *current = selected;
                                selected = (const InvSlot) { 0 };
                                dragging = 0;
                        } else if (current->blockid != 0) {
                                // Pick up item
                                selected = *current;
                                *current = (const InvSlot) { 0 };
                                dragging = 1;
                        }
                }
        }

        // Inventory items
        for (int i = 0; i < INVENTORY_SIZE; i++) {
                InvSlot *current = &player->inventory.slots[i];
        
                if (drawSlot (
                        renderer,
                        current,
                        BUFFER_HALF_W - 76 + (i % HOTBAR_SIZE) * 17,
                        inventoryRect.y + 1 + (i / HOTBAR_SIZE) * 17,
                        inputs->mouse_X,
                        inputs->mouse_Y
                ) && inputs->mouse_Left) {
                        inputs->mouse_Left = 0;
                        if (dragging) {
                                // Place down item
                                *current = selected;
                                selected = (const InvSlot) { 0 };
                                dragging = 0;
                        } else if (current->blockid != 0) {
                                // Pick up item
                                selected = *current;
                                *current = (const InvSlot) { 0 };
                                dragging = 1;
                        }
                }
        }

        if (dragging) {
                drawSlot (
                        renderer,
                        &selected,
                        inputs->mouse_X - 8,
                        inputs->mouse_Y - 8,
                        0, 0
                );
        }
        
        // Exit inventory
        if (inputs->keyboard_E) {
                inputs->keyboard_E = 0;
                inputs->keyTyped   = 0;
                *gamePopup = 0;
        }
}

void popup_chat (SDL_Renderer *renderer, Inputs *inputs, long *gameTime) {
        static int  chatBoxCursor = 0;
        static char chatBox [64]  = {0};
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
        if (inputs->keyTyped || inputs->keySym) {
                if (inputs->keySym == SDLK_BACKSPACE) {
                        // Delete last char and decrement cursor
                        // position
                        if (chatBoxCursor > 0) {
                                chatBox[--chatBoxCursor] = 0;
                        }
                } else if (
                        inputs->keySym == SDLK_RETURN &&
                        chatBoxCursor > 0
                ) {
                        // Add input to chat
                        chatAdd(chatBox);
                        // Clear input box
                        chatBoxCursor = 0;
                        chatBox[0] = 0;
                } else if (
                        inputs->keyTyped > 31 &&
                        inputs->keyTyped < 127 &&
                        chatBoxCursor < 64
                ) {
                        chatBox[chatBoxCursor++] = inputs->keyTyped;
                        chatBox[chatBoxCursor]   = 0;
                }
        }

        // Chat input box
        // If char limit is reached, give some visual
        // feedback.
        if (chatBoxCursor == 64) {
                SDL_SetRenderDrawColor(renderer, 128, 0, 0, 128);
        } else {
                tblack(renderer);
        }
        
        SDL_RenderFillRect(renderer, &chatBoxRect);

        white(renderer);
        drawChar (
                renderer,
                95 + 32 * ((*gameTime >> 6) % 2),
                drawStr (
                        renderer, chatBox,
                        0, BUFFER_H - 8
                ),
                BUFFER_H - 8
        );
}

void popup_pause (
        SDL_Renderer *renderer, Inputs *inputs,
        int *gamePopup, int *gameState
) {
        if (button(renderer, "Back to Game",
                BUFFER_HALF_W - 64, 20, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                *gamePopup = 0;
        }

        if (button(renderer, "Options...",
                BUFFER_HALF_W - 64, 42, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                *gamePopup = 2;
        }

        if (button(renderer, "Quit to Title",
                BUFFER_HALF_W - 64, 64, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                *gameState = 0;
        }
}

void popup_options (
        SDL_Renderer *renderer, Inputs *inputs,
        int *gamePopup, int *drawDistance, int *trapMouse
) {
        if (menu_optionsMain(renderer, inputs, drawDistance, trapMouse)) {
                *gamePopup = 1;
        }
}

#ifndef small
void popup_debugTools (SDL_Renderer *renderer, Inputs *inputs, int *gamePopup) {
        if (button(renderer, "Chunk Peek",
                BUFFER_HALF_W - 64, 20, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                *gamePopup = 5;
        }

        if (button(renderer, "Done",
                BUFFER_HALF_W - 64, 42, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                *gamePopup = 0;
        }
}

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
                if (inputs->mouse_Wheel != 0) {
                        chunkPeekRYMax -= inputs->mouse_Wheel;
                        chunkPeekRYMax = nmod(chunkPeekRYMax, 64);
                        inputs->mouse_Wheel = 0;
                }

                // Mouse for changing chunk map xray
                if (
                        inputs->mouse_X > 128 &&
                        inputs->mouse_Y < 64  &&
                        inputs->mouse_Left
                ) chunkPeekRYMax = inputs->mouse_Y;

                // Up/Down buttons for changing chunk map xray
                if (button(renderer, "UP",
                        4, 56, 64,
                        inputs->mouse_X, inputs->mouse_Y)
                        && inputs->mouse_Left
                ) {
                        chunkPeekRYMax = nmod(chunkPeekRYMax - 1, 64);
                }

                if (button(renderer, "DOWN",
                        4, 78, 64,
                        inputs->mouse_X, inputs->mouse_Y)
                        && inputs->mouse_Left
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
                        chunkPeekColor = textures [
                                debugChunk->blocks [
                                  chunkPeekRX +
                                  (chunkPeekRY << 6) +
                                  (chunkPeekRZ << 12)
                                ] * 256 * 3 + 6 * 16
                        ];
                        if (chunkPeekColor) {
                                SDL_SetRenderDrawColor (
                                        renderer,
                                        (chunkPeekColor >> 16 & 0xFF),
                                        (chunkPeekColor >> 8 & 0xFF),
                                        (chunkPeekColor & 0xFF),
                                        255
                                );
                                SDL_RenderDrawPoint (
                                        renderer,
                                        chunkPeekRX + 128,
                                        chunkPeekRY + chunkPeekRZ
                                );
                                // A little shadow for depth
                                SDL_SetRenderDrawColor (
                                        renderer,
                                        0, 0, 0, 64
                                );
                                SDL_RenderDrawPoint (
                                        renderer,
                                        chunkPeekRX + 128,
                                        chunkPeekRY + chunkPeekRZ + 1
                                );
                        }
                }
        } else {
                drawStr(renderer, "NULL chunk!", 0, 0); 
        }

        if (button(renderer, "Done",
                4, 100, 64,
                inputs->mouse_X, inputs->mouse_Y)
                && inputs->mouse_Left
        ) {
                *gamePopup = 4;
        }
}
#endif

int menu_optionsMain (
        SDL_Renderer *renderer, Inputs *inputs,
        int *drawDistance, int *trapMouse
) {
        static char drawDistanceText [] = "Draw distance: 20\0";
        static char trapMouseText    [] = "Capture mouse: OFF";

        if (button(renderer, drawDistanceText,
                BUFFER_HALF_W - 64, 20, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                switch(*drawDistance) {
                case 20:
                        *drawDistance = 32;
                        break;
                case 32:
                        *drawDistance = 64;
                        break;
                case 64:
                        *drawDistance = 96;
                        break;
                case 96:
                        *drawDistance = 128;
                        break;
                default:
                        *drawDistance = 20;
                        break;
                }
                strnum(drawDistanceText, 15, *drawDistance);
        }

        if (button(renderer, trapMouseText,
                BUFFER_HALF_W - 64, 42, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                if (*trapMouse) {
                        *trapMouse = 0;
                        sprintf(trapMouseText + 15, "OFF");
                } else {
                        *trapMouse = 1;
                        sprintf(trapMouseText + 15, "ON");
                }
        }

        if (button(renderer, "Done",
                BUFFER_HALF_W - 64, 64, 128,
                inputs->mouse_X, inputs->mouse_Y) &&
                inputs->mouse_Left
        ) {
                return 1;
        }

        return 0;
}
