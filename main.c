#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*
  Minecraft 4k, C edition. Version 0.2
  
  Credits:
    Notch - creating the original game
    sashakoshka (holanet.xyz)  - C port
    samsebe (minecraft forums) - deciphering the meaning of some
    of the code
    https://gist.github.com/nowl/828013 - perlin noise
  
  If you distribute a modified copy of this just include this
  notice.
*/

static int randm(int);
static float perlin2d(float, float, int);
static bool setBlock(int*, int, int, int, int);
static void genMap(unsigned int, int, int*);
static void genTextures(unsigned int, int*);
static bool gameLoop(
  int,
  int,
  int,
  unsigned const int,
  int*,
  int*,
  int*,
  SDL_Renderer*
);

int main() {
  int        M[128]    = {0};
  int    world[262144] = {0};
  int textures[12288]  = {0};
  
  //unsigned const int SEED = 18295169;
  unsigned const int SEED = 2048;
  
  const int BUFFER_W     = 214;
  const int BUFFER_H     = 120;
  const int BUFFER_SCALE = 4;
  
  int mouseX, mouseY;
  
  //---- generating assets  ----//
  
  genMap(SEED, 1, world);
  genTextures(SEED, textures);
  
  //----  initializing SDL  ----//
  
  SDL_Window   *window   = NULL;
  SDL_Renderer *renderer = NULL;
  const Uint8  *keyboard = SDL_GetKeyboardState(NULL);
  SDL_Event event;
  
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("cant make window\n");
    goto exit;
  }
  
  window = SDL_CreateWindow("Minecraft",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    BUFFER_W * BUFFER_SCALE, BUFFER_H * BUFFER_SCALE,
    SDL_WINDOW_SHOWN
  );
  if(window == NULL) {
    printf("%s\n", SDL_GetError());
    goto exit;
  }
  
  renderer = SDL_CreateRenderer(
    window,
    -1, 0
  );
  if(renderer == NULL) {
    printf("%s\n", SDL_GetError());
    goto exit;
  }
  
   //----   main game loop   ----//
  
  while(gameLoop(
    BUFFER_W,
    BUFFER_H,
    BUFFER_SCALE,
    SEED,
    M,
    world,
    textures,
    renderer
  )) {
    SDL_PumpEvents();
    
    SDL_GetMouseState(&mouseX, &mouseY);
    M[0] = 0;
    M[1] = 0;
    
    M[2]   = mouseX;
    M[3]   = mouseY;
    
    M[32]  = keyboard[SDL_SCANCODE_SPACE];
    M[119] = keyboard[SDL_SCANCODE_W];
    M[115] = keyboard[SDL_SCANCODE_S];
    M[97]  = keyboard[SDL_SCANCODE_A];
    M[100] = keyboard[SDL_SCANCODE_D];
    
    while(SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          goto exit;
          break;
        case SDL_MOUSEBUTTONDOWN:
          switch(event.button.button) {
            case SDL_BUTTON_LEFT:
              M[1] = 1;
              break;
            case SDL_BUTTON_RIGHT:
              M[0] = 1;
              break;
          }
          break;
      }
    }
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);
  }
  
  exit:
  SDL_Quit();
  return 0;
}

/*
  randm
  Takes in an integer, and uses it as a max for the random
  number it outputs. Supposed to work like Random.nextInt() in
  java.
*/
static int randm(int max) {
  return rand() % max;
}

/*
  genTextures
  Takes in a seed and an array where the textures should go.
  Generates game textures in that array.
*/
static void genTextures(unsigned int SEED, int *textures) {
  srand(SEED);
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

/*
  setBlock
  Takes in a world array, xyz coordinates, and a block id.
  Returns true if the block could be set, otherwise returns
  false.
*/
static bool setBlock(
  int *world,
  int x, int y, int z,
  int block
) {
  if  (x > -1 && x < 64
    && y > -1 && y < 64
    && z > -1 && z < 64
  ) {
    world[x + y * 64 + z * 4096] = block;
    return true;
  } else {
    return false;
  }
}

/*
  genMap
  Takes in a seed and a world array. World is 64x64x64 blocks.
  Fills the world array with generated "terrain".
*/
static void genMap(unsigned int SEED, int type, int *world) {
  srand(SEED);
  static int heightmap[64][64];
  
  switch(type) {
    case 0:
      for(int x = 0; x < 64; x++)
        for(int y = 32; y < 64; y++)
          for(int z = 0; z < 64; z++)
            setBlock(world, x, y, z,
              randm(2) == 0 ? randm(8) : 0);
      break;
    case 1:
      for(int x = 0; x < 64; x++)
        for(int z = 0; z < 64; z++) {
          heightmap[x][z] =
            perlin2d(x, z, SEED) * 16 + 24;
        }
      for(int x = 0; x < 64; x++)
        for(int y = 0; y < 64; y++)
          for(int z = 0; z < 64; z++)
            if(y > heightmap[x][z] + 4)
              setBlock(world, x, y, z, 4);
            else if(y > heightmap[x][z])
              setBlock(world, x, y, z, 2);
            else if(y == heightmap[x][z])
              setBlock(world, x, y, z, 1);
            else
              setBlock(world, x, y, z, 0);
      break;
  }
}

/*
  gameLoop
  Does all the raycasting stuff, moves the player around, etc.
  If by chance the game ends, it returns false - which should
  terminate the main while loop and end the program.
*/
static bool gameLoop(
  int BUFFER_W,
  int BUFFER_H,
  int BUFFER_SCALE,
  unsigned int SEED,
  int *M,
  int *world,
  int *textures,
  SDL_Renderer *renderer
) {
  // We dont want to have to pass all of these by reference, so
  // have all of them as static variables and only set their
  // values once.
  static float  f1,
                f2,
                f3,
                f4,
                f5,
                f6,
                f7,
                f8,
                f9,
                f10,
                f11,
                f12,
                f13,
                f14,
                f15,
                f16,
                f17,
                f18,
                f19,
                f20,
                f21,
                f22,
                f23,
                f24,
                f25,
                f26,
                f27,
                f28,
                f29,
                f30,
                f31,
                f32,
                f33,
                f34,
                f35,
                f36;
  
  static long   l;
  
  static int    k,
                m,
                pointX,
                pointY,
                pointXMax,
                pointYMax,
                xBegin,
                yBegin,
                i4,
                i5,
                i6,
                i7,
                i8,
                i9,
                i10,
                i11,
                i12,
                i13,
                i14,
                i15,
                i16,
                i17,
                i18,
                i19,
                i20,
                i21,
                i22,
                i23,
                i24,
                i25,
                pixelColor;
  
  static double d;
  
  static bool init = true;
  if(init) {
    f1 = 96.5F;
    f2 = 65.0F;
    f3 = 96.5F;
    f4 = 0.0F;
    f5 = 0.0F;
    f6 = 0.0F;
    i4 = -1;
    i5 = 0;
    f7 = 0.0F;
    f8 = 0.0F;
    l = SDL_GetTicks();
  }
  
  // TODO: port main game loop, with repaired controls. ALL VARS
  // MUST BE STATIC!!!!!!!!!!!!!!!!!
  
  f9  = sin(f7),
  f10 = cos(f7),
  f11 = sin(f8),
  f12 = cos(f8);
  
  // Proccess user input
  
  do {
    // Looking around
    f16 = (M[2] - BUFFER_W * 2) / 214.0F * 2.0F;
    f17 = (M[3] - BUFFER_H * 2) / 120.0F * 2.0F;
    f15 = sqrt(f16 * f16 + f17 * f17) - 1.2F;
    if (f15 < 0.0F)
      f15 = 0.0F;
    if (f15 > 0.0F) {
      f7 += f16 * f15 / 400.0F;
      f8 -= f17 * f15 / 400.0F;
      if (f8 < -1.57F)
        f8 = -1.57F;
      if (f8 > 1.57F)
        f8 = 1.57F;
    }
    
    // Moving around
    l += 12;
    f13 = 0.0;
    f14 = 0.0;
    f14 += (M[119] - M[115]) * 0.02;
    f13 += (M[100] - M[97]) * 0.02;
    f4 *= 0.5;
    f5 *= 0.99;
    f6 *= 0.5;
    f4 += f9 * f14 + f10 * f13;
    f6 += f10 * f14 - f9 * f13;
    f5 += 0.003;
    
    for (m = 0; m < 3; m++) {
      f16 = f1 + f4 * ((m + 2) % 3 / 2);
      f17 = f2 + f5 * ((m + 1) % 3 / 2);
      f19 = f3 + f6 * ((m + 2) % 3 / 2);
      for (i12 = 0; i12 < 12; i12++) {
        i13 = (int)(f16 + (i12 >> 0 & 0x1) * 0.6F - 0.3F) - 64;
        i14 = (int)(f17 + ((i12 >> 2) - 1) * 0.8F + 0.65F) - 64;
        i15 = (int)(f19 + (i12 >> 1 & 0x1) * 0.6F - 0.3F) - 64;
        if (i13 < 0
          || i14 < 0
          || i15 < 0
          || i13 >= 64
          || i14 >= 64
          || i15 >= 64
          || world[i13 + i14 * 64 + i15 * 4096] > 0
        ) {
          if (m != 1) {
            goto label208;
          }
          if (M[32] > 0 && f5 > 0.0F) {
            M[32] = 0;
            f5 = -0.1F;
            goto label208;
          } 
          f5 = 0.0F;
          goto label208;
        }
      }
      f1 = f16;
      f2 = f17;
      f3 = f19;
    }
    label208:;
  } while (SDL_GetTicks() - l > 4);
  
  i6 = 0;
  i7 = 0;
  if (M[1] > 0 && i4 > 0) {
    world[i4] = 0;
    M[1] = 0;
  } 
  if (M[0] > 0 && i4 > 0) {
    world[i4 + i5] = 1;
    M[0] = 0;
  }
  for (k = 0; k < 12; k++) {
    m = (int)(f1 + (k >> 0 & 0x1) * 0.6F - 0.3F) - 64;
    i10 = (int)(f2 + ((k >> 2) - 1) * 0.8F + 0.65F) - 64;
    i11 = (int)(f3 + (k >> 1 & 0x1) * 0.6F - 0.3F) - 64;
    if (m >= 0
      && i10 >= 0
      && i11 >= 0
      && m   < 64
      && i10 < 64
      && i11 < 64
    ) {
      setBlock(world, m, i10, i11, 0);
    }
  }
  
  i8 = -1.0F;
  for (i9 = 0; i9 < BUFFER_W; i9++) {
    f18 = (i9 - 107) / 90.0F;
    for (i11 = 0; i11 < 120; i11++) {
      f20 = (i11 - 60) / 90.0F;
      f21 = 1.0F;
      f22 = f21 * f12 + f20 * f11;
      f23 = f20 * f12 - f21 * f11;
      f24 = f18 * f10 + f22 * f9;
      f25 = f22 * f10 - f18 * f9;
      i16 = 0;
      i17 = 255;
      d = 20.0D;
      f26 = 5.0F;
      for (i18 = 0; i18 < 3; i18++) {
        f27 = f24;
        if (i18 == 1)
          f27 = f23; 
        if (i18 == 2)
          f27 = f25; 
        f28 = 1.0F / ((f27 < 0.0F) ? -f27 : f27);
        f29 = f24 * f28;
        f30 = f23 * f28;
        f31 = f25 * f28;
        f32 = f1 - (int)f1;
        if (i18 == 1)
          f32 = f2 - (int)f2; 
        if (i18 == 2)
          f32 = f3 - (int)f3; 
        if (f27 > 0.0F)
          f32 = 1.0F - f32; 
        f33 = f28 * f32;
        f34 = f1 + f29 * f32;
        f35 = f2 + f30 * f32;
        f36 = f3 + f31 * f32;
        if (f27 < 0.0F) {
          if (i18 == 0)
            f34--; 
          if (i18 == 1)
            f35--; 
          if (i18 == 2)
            f36--; 
        } 
        while (f33 < d) {
          i21 = (int)f34 - 64;
          i22 = (int)f35 - 64;
          i23 = (int)f36 - 64;
          if (i21 < 0
            || i22 < 0
            || i23 < 0
            || i21 >= 64
            || i22 >= 64
            || i23 >= 64
          ) {
            break;
          }
          i24 = i21 + i22 * 64 + i23 * 4096;
          i25 = world[i24];
          if (i25 > 0) {
            i6 = (int)((f34 + f36) * 16.0F) & 0xF;
            i7 = ((int)(f35 * 16.0F) & 0xF) + 16;
            if (i18 == 1) {
              i6 = (int)(f34 * 16.0F) & 0xF;
              i7 = (int)(f36 * 16.0F) & 0xF;
              if (f30 < 0.0F)
                i7 += 32; 
            }
            // Block outline color
            pixelColor = 0xFFFFFF;
            if (i24 != i4
              || (i6 > 0
                && i7 % 16 > 0
                && i6 < 15
                && i7 % 16 < 15
              )
            ) {
              pixelColor = textures[
                i6 + i7 * 16 + i25 * 256 * 3
              ]; 
            }
            if (f33 < f26 && i9 == M[2] / 4 && i11 == M[3] / 4) {
              i8 = i24;
              i5 = 1;
              if (f27 > 0.0F)
                i5 = -1; 
              i5 <<= 6 * i18;
              f26 = f33;
            } 
            if (pixelColor > 0) {
              i16 = pixelColor;
              i17 = 255 - (int)(f33 / 20.0F * 255.0F);
              i17 = i17 * (255 - (i18 + 2) % 3 * 50) / 255;
              d = f33;
            } 
          } 
          f34 += f29;
          f35 += f30;
          f36 += f31;
          f33 += f28;
        } 
      }
      
      // getting pixel RGB
      i18 = (i16 >> 16 & 0xFF) * i17 / 255;
      i19 = (i16 >> 8 & 0xFF)  * i17 / 255;
      i20 = (i16 & 0xFF)       * i17 / 255;
      
      xBegin = i9  * BUFFER_SCALE;
      yBegin = i11 * BUFFER_SCALE;
      pointXMax = xBegin + BUFFER_SCALE;
      pointYMax = yBegin + BUFFER_SCALE;
      
      SDL_SetRenderDrawColor(renderer, i18, i19, i20, 255);
      for(pointX = xBegin; pointX < pointXMax; pointX++) {
        for(pointY = yBegin; pointY < pointYMax; pointY++) {
          SDL_RenderDrawPoint(renderer, pointX, pointY);
        }
      }
      
      i4 = i8;
    }
  }
  
  init = false;
  return true;
}

static int noise2(int x, int y, Uint8 *hash, int seed) {
  static int tmp;
  tmp = hash[(y + seed) % 256];
  return hash[(tmp + x) % 256];
}

static float smooth_inter(float x, float y, float s) {
  return x + s * s * (3 - 2 * s) * (y - x);
}

static float perlin2d(float x, float y, int seed) {
  static Uint8 hash[] =
  
  {208,34,231,213,32,248,233,56,161,78,24,140,
  71,48,140,254,245,255,247,247,40,185,248,251,245,28,124,204,
  204,76,36,1,107,28,234,163,202,224,245,128,167,204,9,92,217,
  54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,
  204,8,81,70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,
  169,112,32,97,53,195,13,203,9,47,104,125,117,114,124,165,203,
  181,235,193,206,70,180,174,0,167,181,41,164,30,116,127,198,245,
  146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,228,108,
  245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,
  237,55,89,232,50,217,64,244,157,199,121,252,90,17,212,203,149,
  152,140,187,234,177,73,174,193,100,192,143,97,53,145,135,19,
  103,13,90,135,151,199,91,239,247,33,39,145,101,120,99,3,186,86,
  99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,135,
  176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,
  137,139,255,114,20,218,113,154,27,127,246,250,1,8,198,250,209,
  92,222,173,21,88,102,219};
  
  float xa = x * 0.0625;
  float ya = y * 0.0625;
  float amp = 1.0;
  float fin = 0;
  float div = 0.0;

  int i;
  for(i = 0; i < 4; i++) {
    div += 256 * amp;
    
    int x_int = xa;
    int y_int = ya;
    float x_frac = xa - x_int;
    float y_frac = ya - y_int;
    int s = noise2(x_int, y_int, hash, seed);
    int t = noise2(x_int + 1, y_int,     hash, seed);
    int u = noise2(x_int,     y_int + 1, hash, seed);
    int v = noise2(x_int + 1, y_int + 1, hash, seed);
    float low  = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    
    fin += smooth_inter(low, high, y_frac) * amp;
    amp /= 2;
    xa *= 2;
    ya *= 2;
  }

  return fin/div;
}
