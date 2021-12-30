#pragma once

#include <SDL2/SDL.h>
#include "utility.h"
#include "main.h"
#include "gui.h"
#include "terrain.h"
#include "textures.h"
#include "menus.h"

int gameLoop(
  unsigned int seed,
  Inputs *inputs,
  World *world,
  SDL_Renderer *renderer
);
