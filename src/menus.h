#pragma once

#include "gui.h"
#include "terrain.h"
#include "main.h"

#define STATE_TITLE        0
#define STATE_ABOUT        1
#define STATE_SELECT_WORLD 2
#define STATE_NEW_WORLD    3
#define STATE_LOADING      4
#define STATE_GAMEPLAY     5
#define STATE_EDIT_WORLD   6
#define STATE_JOIN_GAME    7
#define STATE_OPTIONS      8

#define POPUP_HUD            0
#define POPUP_PAUSE          1
#define POPUP_OPTIONS        2
#define POPUP_INVENTORY      3
#define POPUP_ADVANCED_DEBUG 4
#define POPUP_CHUNK_PEEK     5
#define POPUP_CHAT           6

int  state_title       (SDL_Renderer *, Inputs *, int *);
void state_selectWorld (SDL_Renderer *, Inputs *, int *);
void state_newWorld    (SDL_Renderer *, Inputs *, int *, int *, int *, int *);
int  state_loading     (SDL_Renderer *, World *, unsigned int, Coords);
void state_options     (SDL_Renderer *, Inputs *, int *);
void state_egg         (SDL_Renderer *, Inputs *, int *);
int  state_err         (SDL_Renderer *, Inputs *, char *);

void popup_hud (
        SDL_Renderer *, Inputs *, World *,
        int *, u_int32_t *, Player *
);
void popup_inventory  (SDL_Renderer *, Inputs *, Player *, int *);
void popup_chat       (SDL_Renderer *, Inputs *, u_int64_t);
void popup_pause      (SDL_Renderer *, Inputs *, int *, int *);
void popup_options    (SDL_Renderer *, Inputs *, int *);
void popup_debugTools (SDL_Renderer *, Inputs *, int *);
void popup_chunkPeek  (SDL_Renderer *, Inputs *, World *, int *, Player *);
