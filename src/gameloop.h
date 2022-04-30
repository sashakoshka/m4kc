#pragma once

#include <SDL2/SDL.h>
#include "terrain.h"
#include "main.h"
#include "player.h"

int  gameLoop                 (Inputs *inputs, SDL_Renderer *renderer);
void gameLoop_processMovement (Inputs *);
void gameLoop_resetGame       ();
int screenshot ();
