#pragma once

#include "gui.h"
#include "terrain.h"
#include "main.h"

int  state_title      (SDL_Renderer *, Inputs *, int *);
void state_newWorld   (SDL_Renderer *, Inputs *, int *, int *, int *, int *);
int  state_loading    (SDL_Renderer *, World *, unsigned int, Coords);
void state_options    (SDL_Renderer *, Inputs *, int *);
void state_egg        (SDL_Renderer *, Inputs *, int *);
int  state_err        (SDL_Renderer *, Inputs *, char *);

void popup_hud (
        SDL_Renderer *, Inputs *, World *world,
        int *, u_int32_t *, Player *
);
void popup_inventory  (SDL_Renderer *, Inputs *, Player *, int *);
void popup_chat       (SDL_Renderer *, Inputs *, long *);
void popup_pause      (SDL_Renderer *, Inputs *, int *, int *);
void popup_options    (SDL_Renderer *, Inputs *, int *);
void popup_debugTools (SDL_Renderer *, Inputs *, int *);
void popup_chunkPeek  (SDL_Renderer *, Inputs *, World *, int *, Player *);
