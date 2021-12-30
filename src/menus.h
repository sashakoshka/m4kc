#pragma once

#include "gui.h"
#include "terrain.h"
#include "main.h"

int  state_title      (SDL_Renderer *, Inputs *, int *, int *);
void state_options    (SDL_Renderer *, Inputs *, int *, int *, int *);
void state_egg        (SDL_Renderer *, Inputs *, int *);

void popup_hud (
  SDL_Renderer *, Inputs *, World *world,
  int *, u_int32_t *, Player *
);
void popup_chat       (SDL_Renderer *renderer, Inputs *inputs, long *gameTime);
void popup_pause      (SDL_Renderer *, Inputs *, int *, int *);
void popup_options    (SDL_Renderer *, Inputs *, int *, int *, int *);
void popup_debugTools (SDL_Renderer *, Inputs *, int *);
void popup_chunkPeek  (SDL_Renderer *, Inputs *, World *, int *, Player *);
