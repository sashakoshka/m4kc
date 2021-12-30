#pragma once

#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "player.h"
#include "minecraftfont.h"
#include "textures.h"

void strnum          (char*, int, int);
int  drawChar        (SDL_Renderer*,   int, int, int);
int  drawStr         (SDL_Renderer*, char*, int, int);
int  shadowStr       (SDL_Renderer*, char*, int, int);
int  centerStr       (SDL_Renderer*, char*, int, int);
int  shadowCenterStr (SDL_Renderer*, char*, int, int);
int  drawBig         (SDL_Renderer*, char*, int, int);
int  drawBGStr       (SDL_Renderer*, char*, int, int);
int  button          (SDL_Renderer*, char*, int, int, int, int, int);
int  drawSlot        (SDL_Renderer*, InvSlot*, int, int, int, int);
void dirtBg          (SDL_Renderer*);
void loadScreen      (SDL_Renderer*, char*, float, float);
void chatAdd         (char*);
void white           (SDL_Renderer*);
void tblack          (SDL_Renderer*);

extern const int BUFFER_W;
extern const int BUFFER_H;
extern const int BUFFER_SCALE;
extern const int BUFFER_HALF_W;
extern const int BUFFER_HALF_H;

extern char chatHistory      [11][64];
extern int  chatHistoryFade  [11];
extern int  chatHistoryIndex;
