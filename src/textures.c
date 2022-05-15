#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "textures.h"

int textures[TEXTURES_SIZE] = { 0 };
const uint16_t cobbleCracks[BLOCK_TEXTURE_H] = {
        0b0000001110000100,
        0b0010110010000110,
        0b1011100011001110,
        0b1110100011110011,
        0b0011000110001001,
        0b0001000100001111,
        0b0001111110000001,
        0b0011001111100110,
        0b1110001000111100,
        0b0100010100011000,
        0b1000010000011100,
        0b0100110000110111,
        0b0011111011000010,
        0b1100001010000001,
        0b0010000111000011,
        0b0000111100111110,
};

static void genTexture (Block);
static inline float determ2d (float, float);

/* genTextures
 * Takes in a seed and an array where the textures should go.
 * Generates game textures in that array.
 */
void genTextures (unsigned int seed) {
        srand(seed);

        for (Block blockId = 1; blockId < NUMBER_OF_BLOCKS; blockId++) {
                genTexture(blockId);
        }
}

/* genTexture
 * Generates a texture for the given block ID.
 */
static void genTexture (Block blockId) {
        int brightness = 255 - randm(96);
        for (int y = 0; y < BLOCK_TEXTURE_H * 3; y++)
        for (int x = 0; x < BLOCK_TEXTURE_W;     x++) {
                int baseColor  = 0x966C4A;
                int noiseFloor = 255;
                int noiseScale = 96;

                if (blockId == BLOCK_SAND)   { noiseScale = 48;  }
                if (blockId == BLOCK_GRAVEL) { noiseScale = 140; }

                if (
                        blockId == BLOCK_GRASS &&
                        y < (x * x * (3 + x) * 81 >> 2 & 0x3) + 18
                ) {
                        baseColor = 0x6AAA40;
                } else if (
                        blockId == BLOCK_GRASS &&
                        y < (x * x * (3 + x) * 81 >> 2 & 0x3) + 19
                ) {
                        brightness = brightness * 2 / 3;
                }

                int needAltNoise =
                        blockId == BLOCK_STONE ||
                        blockId == BLOCK_WATER;
                if (!needAltNoise || randm(3) == 0) {
                        brightness = noiseFloor - randm(noiseScale);
                }

                if (blockId == BLOCK_WOOD) {
                        baseColor = 0x675231;
                        if (
                                x > 0 && x < 15 &&
                                ((y > 0 && y < 15) || (y > 32 && y < 47))
                        ) {
                                baseColor = 0xBC9862;
                                int i6 = x - 7;
                                int i7 = (y & 0xF) - 7;

                                if (i6 < 0)  i6 = 1 - i6;
                                if (i7 < 0)  i7 = 1 - i7;
                                if (i7 > i6) i6 = i7;

                                brightness = 196 - randm(32) + i6 % 3 * 32;
                        } else if (randm(2) == 0) {
                                brightness =
                                        brightness *
                                        (150 - (x & 0x1) * 100) / 100;
                        }
                }

                switch (blockId) {
                case BLOCK_STONE:
                        baseColor = 0x7F7F7F;
                        break;
                        
                case BLOCK_SAND:
                        baseColor = 0xD8CE9B;
                        break;
                        
                case BLOCK_GRAVEL:
                        baseColor = 0xAAAAAA;
                        break;
                        
                case BLOCK_BRICKS:
                        baseColor = 0xB53A15;
                        if ((x + y / 4 * 4) % 8 == 0 || y % 4 == 0)
                        baseColor = 12365733; 
                        break;
                        
                case BLOCK_COBBLESTONE:
                        baseColor = 0x999999;
                        brightness -=
                                ((cobbleCracks[y & 0xF] >> x) & 1) * 128;
                        break;

                case BLOCK_WATER:
                        baseColor = 0x3355EE;
                        break;
                        
                case BLOCK_PLAYER_HEAD:
                        brightness = 255;
                        if (
                                dist2d(x, y % BLOCK_TEXTURE_H, 8, 8) > 6.2 ||
                                (y / 16) % 3 == 2
                        ) {
                                baseColor = 0x000000;
                        } else {
                                baseColor = 0xFFFFFF;
                                brightness -= dist2d (
                                        x, y % BLOCK_TEXTURE_H,
                                        8, 2) * 8;
                        }
                        break;
                        
                case BLOCK_PLAYER_BODY:
                        brightness = 255;
                        if (
                                (dist2d(x, y % BLOCK_TEXTURE_H, 8, 16) > 12.2 ||
                                (y / 16) % 3 != 1) &&
                                (y / 16) % 3 != 2
                        ) {
                                baseColor = 0x000000;
                        } else {
                                baseColor = 0xFFFFFF;
                                brightness -= dist2d (
                                        x, y % BLOCK_TEXTURE_H,
                                        8, 2) * 8;
                        }
                        break;
                
                case BLOCK_LEAVES:
                        baseColor = 0x50D937;

                        // Make transparent gaps between leaves
                        if (randm(2) == 0) {
                                baseColor = 0;
                                brightness = 255;
                        }
                        break;
                        
                case BLOCK_TALL_GRASS:
                        baseColor = 0x50D937;

                        // Make transparent gaps between blades of grass, and
                        // make top transparent
                        if (
                                determ2d(x, y / 3) < 0.2 ||
                                y < BLOCK_TEXTURE_H      ||
                                randm(y - BLOCK_TEXTURE_H + 1) < 2
                        ) {
                                baseColor = 0;
                                brightness = 255;
                        }
                        break;
                }

                // Darken bottom of blocks. We need finalBrightness because
                // brightness can carry over between pixels.
                int finalBrightness = brightness;
                if (y >= BLOCK_TEXTURE_H * 2) {
                        finalBrightness /= 2;
                }

                // Apply brightness value
                int finalColor =
                        (baseColor >> 16 & 0xFF) * finalBrightness / 255 << 16 |
                        (baseColor >> 8  & 0xFF) * finalBrightness / 255 << 8  |
                        (baseColor       & 0xFF) * finalBrightness / 255;
                
                textures [
                        x +
                        y * BLOCK_TEXTURE_H +
                        blockId * BLOCK_TEXTURE_W * BLOCK_TEXTURE_H * 3
                ] = finalColor;
        }
}

/* determ2d
 * A deterministic pseudorandom noise generator separate from randm. Takes in
 * an x and y value.
 */
static inline float determ2d (float x, float y) {
        return fmod(fabs(tan(9 * (float)x + 1 + y)), 1);
}
