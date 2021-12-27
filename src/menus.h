#pragma once

#include "gui.h"

void menu_hud (
  SDL_Renderer *, Inputs *,
  int *, u_int32_t *,
  Inventory *, Coords *
);

void menu_chat (SDL_Renderer *renderer, Inputs *inputs, long *gameTime);
