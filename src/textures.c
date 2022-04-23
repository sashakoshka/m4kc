#include "textures.h"

int textures[12288] = {0};
const u_int16_t cobbleCracks[16] = {
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

/* genTextures
 * Takes in a seed and an array where the textures should go.
 * Generates game textures in that array.
 */
void genTextures(unsigned int seed) {
        srand(seed);

        for (int blockId = 1; blockId < 16; blockId++) {
                genTexture(blockId);
        }
}

void genTexture (int blockId) {
        int k = 255 - randm(96);
        for (int y = 0; y < 48; y++)
        for (int x = 0; x < 16; x++) {
                int baseColor = 9858122;
                int noiseFloor = 255;
                int noiseScale = 96;

                // sand
                if (blockId == 3) {
                        baseColor = 0xd8ce9b;
                        noiseScale = 48;
                }

                // stone
                if (blockId == 4)
                        baseColor = 8355711;

                // gravel
                if (blockId == 6) {
                        baseColor = 0xAAAAAA;
                        noiseScale = 140;
                }

                // add noise
                if (blockId != 4 || randm(3) == 0)
                        k = noiseFloor - randm(noiseScale);

                // grass
                if (
                        blockId == 1 &&
                        y < (x * x * (3 + x) * 81 >> 2 & 0x3) + 18
                ) {
                        baseColor = 6990400;
                } else if (
                        blockId == 1 &&
                        y < (x * x * (3 + x) * 81 >> 2 & 0x3) + 19
                ) {
                        k = k * 2 / 3;
                }

                // logs
                if (blockId == 7) {
                        baseColor = 6771249;
                        if (
                                x > 0 && x < 15 &&
                                ((y > 0 && y < 15) || (y > 32 && y < 47))
                        ) {
                                baseColor = 12359778;
                                int i6 = x - 7;
                                int i7 = (y & 0xF) - 7;

                                if (i6 < 0)  i6 = 1 - i6;
                                if (i7 < 0)  i7 = 1 - i7;
                                if (i7 > i6) i6 = i7;

                                k = 196 - randm(32) + i6 % 3 * 32;
                        } else if (randm(2) == 0) {
                                k = k * (150 - (x & 0x1) * 100) / 100;
                        }
                }

                // bricks
                if (blockId == 5) {
                        baseColor = 11876885;
                        if ((x + y / 4 * 4) % 8 == 0 || y % 4 == 0)
                        baseColor = 12365733; 
                }

                // cobblestone
                if (blockId == 9) {
                        baseColor = 0x999999;
                        k -= ((cobbleCracks[y & 0xF] >> x) & 0b1) * 128;
                }

                int i2 = k;
                if (y >= 32)
                i2 /= 2; 
                if (blockId == 8) {
                        baseColor = 5298487;
                        if (randm(2) == 0) {
                                baseColor = 0;
                                i2 = 255;
                        }
                }

                // darken bottom of blocks
                int i3 = (baseColor >> 16 & 0xFF)
                * i2 / 255 << 16 | (baseColor >> 8 & 0xFF)
                * i2 / 255 << 8  | (baseColor & 0xFF)
                * i2 / 255;
                textures[x + y * 16 + blockId * 256 * 3] = i3;
        }
}