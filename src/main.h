#pragma once

#include <SDL2/SDL.h>
#include <stdlib.h>

/* InputKeys
 * Stores only key presses. Reset to zero every frame.
 */
typedef struct {
        int Space;  // M[32]
        int W;      // M[119]
        int S;      // M[115]
        int A;      // M[97]
        int D;      // M[100]
        int E;
        int T;
        int F;

        int Esc;    // M[27]
        int F1;     // M[5]
        int F2;     // M[6]
        int F3;     // M[7]
        int F4;

        int num0;
        int num1;
        int num2;
        int num3;
        int num4;
        int num5;
        int num6;
        int num7;
        int num8;
        int num9;
} InputKeys;

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

        InputKeys keyboard;

        // These are specifically for text input fields
        int keyTyped;
        int keySym;
} Inputs;

int controlLoop(Inputs *inputs, const u_int8_t *keyboard);
int handleEvent(Inputs *inputs, const u_int8_t *keyboard, SDL_Event event);
