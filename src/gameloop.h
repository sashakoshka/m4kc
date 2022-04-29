#pragma once

#include <SDL2/SDL.h>
#include "terrain.h"
#include "main.h"
#include "player.h"

int gameLoop (
        unsigned int seed,
        Inputs *inputs,
        World *world,
        SDL_Renderer *renderer
);
void gameLoop_processMovement (Inputs *, World *, Player *);
void gameLoop_resetGame       ();
int screenshot ();
