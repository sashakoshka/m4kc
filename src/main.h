#pragma once

#include <SDL2/SDL.h>
#include <stdlib.h>

/* InputKeys
 * Stores only key presses. Reset to zero every frame.
 */
typedef struct {
        int space;  // M[32]
        int w;      // M[119]
        int s;      // M[115]
        int a;      // M[97]
        int d;      // M[100]
        int e;
        int t;
        int f;

        int esc;    // M[27]
        int f1;     // M[5]
        int f2;     // M[6]
        int f3;     // M[7]
        int f4;

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

typedef struct {
        int right;     // M[0]
        int left;      // M[1]
        int x;         // M[2]
        int y;         // M[3]
        int wheel;     // M[4]
} InputMouse;

/* Inputs
 * Stores the state of the different inputs (keyboard/mouse) for
 * the current tick. Updated on each game loop.
 */
typedef struct {
        InputMouse mouse;
        InputKeys  keyboard;

        // These are specifically for text input fields
        int keyTyped;
        int keySym;
} Inputs;
