#pragma once

#include <SDL2/SDL.h>
#include "terrain.h"
#include "main.h"
#include "player.h"

#define MAX_FPS 60
#define MIN_FRAME_MILLISECONDS 1000 / MAX_FPS

int gameLoop (
        unsigned int seed,
        Inputs *inputs,
        SDL_Renderer *renderer
);
void gameLoop_processMovement (Inputs *);
void gameLoop_resetGame       ();
int screenshot ();
