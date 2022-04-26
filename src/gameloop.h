#pragma once

#include <SDL2/SDL.h>
#include "terrain.h"
#include "main.h"

int gameLoop (
        unsigned int seed,
        Inputs *inputs,
        World *world,
        SDL_Renderer *renderer
);

int screenshot ();
