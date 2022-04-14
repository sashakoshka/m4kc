#pragma once

#include <stdlib.h>

/* Inputs
 * Stores the state of the different inputs (keyboard/mouse) for
 * the current tick. Updated on each game loop.
 */
typedef struct {
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
        int keyboard_E;
        int keyboard_T;

        int keyboard_Esc;    // M[27]
        int keyboard_F1;     // M[5]
        int keyboard_F2;     // M[6]
        int keyboard_F3;     // M[7]
        int keyboard_F4;

        int keyTyped;
        int keySym;
} Inputs;

int controlLoop(Inputs *inputs, const u_int8_t *keyboard);
int handleEvent(Inputs *inputs, const u_int8_t *keyboard, SDL_Event event);
