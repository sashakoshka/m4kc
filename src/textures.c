#include "textures.h"

int textures[12288] = {0};

/*
  genTextures
  Takes in a seed and an array where the textures should go.
  Generates game textures in that array.
*/
void genTextures(unsigned int seed) {
  srand(seed);
  static int j  = 0,
             k  = 0,
             m  = 0,
             n  = 0,
             i1 = 0,
             i2 = 0,
             i3 = 0,
             i6 = 0,
             i7 = 0;
  
  for (j = 1; j < 16; j++) {
    k = 255 - randm(96);
    for (m = 0; m < 48; m++) {
      for (n = 0; n < 16; n++) {
        i1 = 9858122;
        if (j == 4)
          i1 = 8355711; 
        if (j != 4 || randm(3) == 0)
          k = 255 - randm(96); 
        if (j == 1
          && m < (n * n * (3 + n) * 81 >> 2 & 0x3) + 18)
        {
          i1 = 6990400;
        } else if (j == 1
          && m < (n * n * (3 + n) * 81 >> 2 & 0x3) + 19)
        {
          k = k * 2 / 3;
        } 
        if (j == 7) {
          i1 = 6771249;
          if (n > 0 && n < 15
            && ((m > 0 && m < 15) || (m > 32 && m < 47)))
          {
            i1 = 12359778;
            i6 = n - 7;
            i7 = (m & 0xF) - 7;
            if (i6 < 0)
              i6 = 1 - i6; 
            if (i7 < 0)
              i7 = 1 - i7; 
            if (i7 > i6)
              i6 = i7; 
            k = 196 - randm(32) + i6 % 3 * 32;
          } else if (randm(2) == 0) {
            k = k * (150 - (n & 0x1) * 100) / 100;
          } 
        } 
        if (j == 5) {
          i1 = 11876885;
          if ((n + m / 4 * 4) % 8 == 0 || m % 4 == 0)
            i1 = 12365733; 
        } 
        i2 = k;
        if (m >= 32)
          i2 /= 2; 
        if (j == 8) {
          i1 = 5298487;
          if (randm(2) == 0) {
            i1 = 0;
            i2 = 255;
          }
        }
        i3 = (i1 >> 16 & 0xFF)
          * i2 / 255 << 16 | (i1 >> 8 & 0xFF)
          * i2 / 255 << 8 | (i1 & 0xFF)
          * i2 / 255;
        textures[n + m * 16 + j * 256 * 3] = i3;
      }
    }
  }
}

