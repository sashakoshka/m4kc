#pragma once

#include "utility.h"
#include "blocks.h"

#define BLOCK_TEXTURE_W  16
#define BLOCK_TEXTURE_H  16
#define TEXTURES_SIZE NUMBER_OF_BLOCKS * BLOCK_TEXTURE_W * BLOCK_TEXTURE_H * 3

extern int textures[TEXTURES_SIZE];
void genTextures (unsigned int);
