#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "structs.h"
#include "utility.h"
#include "textures.h"
#include "minecraftfont.h"
#include "terrain.h"
#include "gui.h"

/*
  Minecraft 4k, C edition. Version 0.5
  
  Credits:
    notch       - creating the original game
    sashakoshka - C port, modifications
    samsebe     - deciphering the meaning of some of the code
    gracie bell - daylight function
    segfault    - makefile
    https://gist.github.com/nowl/828013 - perlin noise
    samantaz    - de-obfuscating controls code
  
  If you distribute a modified copy of this, just include this
  notice.
*/

int gameLoop(
  int,
  int,
  int,
  unsigned int,
  Inputs*,
  World*,
  SDL_Renderer*,
  SDL_Window*
);

int main(int argc, char *argv[]) {
  Inputs inputs = {0};
  
  World world;
  
  //unsigned int seed = 18295169;
  unsigned int seed = 45390874;
  
  const int BUFFER_W     = 214;
  const int BUFFER_H     = 120;
  const int BUFFER_SCALE = 4;
  
  int mouseX = 0, mouseY = 0;
  
  //---- generating assets  ----//
  
  initChunks(&world);
  
  //genChunk(&world, seed, 0, 0, 0, 1);
  genAll(&world, seed, 1);
  genTextures(seed);
  
  //----  initializing SDL  ----//
  
  // There are a couple things here that are commented out to
  // cut down executable size. If you are trying to solve a
  // problem, just comment them back in.
  
  SDL_Window   *window   = NULL;
  SDL_Renderer *renderer = NULL;
  const Uint8  *keyboard = SDL_GetKeyboardState(NULL);
  SDL_Event event;
  
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    //printf("cant make window\n");
    goto exit;
  }
  
  window = SDL_CreateWindow("M4KC",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    BUFFER_W * BUFFER_SCALE, BUFFER_H * BUFFER_SCALE,
    SDL_WINDOW_SHOWN
  );
  if(window == NULL) {
    //printf("%s\n", SDL_GetError());
    goto exit;
  }
  
  renderer = SDL_CreateRenderer(
    window,
    -1, 0
  );
  /*
  if(renderer == NULL) {
    printf("%s\n", SDL_GetError());
    goto exit;
  }
  */
  SDL_RenderSetScale(renderer, BUFFER_SCALE, BUFFER_SCALE);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  
   //----   main game loop   ----//
  
  while(gameLoop(
    BUFFER_W,
    BUFFER_H,
    BUFFER_SCALE,
    seed,
    &inputs,
    &world,
    renderer,
    window
  )) {
    
    SDL_PumpEvents();
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // Detect movement controls
    inputs.keyboard_Space  = keyboard[SDL_SCANCODE_SPACE];
    inputs.keyboard_W      = keyboard[SDL_SCANCODE_W];
    inputs.keyboard_S      = keyboard[SDL_SCANCODE_S];
    inputs.keyboard_A      = keyboard[SDL_SCANCODE_A];
    inputs.keyboard_D      = keyboard[SDL_SCANCODE_D];
    
    if(!SDL_GetRelativeMouseMode()) {
      inputs.mouse_X = mouseX;
      inputs.mouse_Y = mouseY;
    }
    
    while(SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          goto exit;
          
        case SDL_MOUSEBUTTONDOWN:
          switch(event.button.button) {
            case SDL_BUTTON_LEFT:
              inputs.mouse_Left = 1;
              break;
            case SDL_BUTTON_RIGHT:
              inputs.mouse_Right = 1;
              break;
          }
          break;
        
        case SDL_KEYDOWN:
          if(event.key.repeat == 0) {
            // Detect UI hotkeys
            inputs.keyboard_Esc = keyboard[SDL_SCANCODE_ESCAPE];
            inputs.keyboard_F1  = keyboard[SDL_SCANCODE_F1];
            inputs.keyboard_F2  = keyboard[SDL_SCANCODE_F2];
            inputs.keyboard_F3  = keyboard[SDL_SCANCODE_F3];
          }
          break;
        
        case SDL_MOUSEWHEEL:
          inputs.mouse_Wheel = event.wheel.y;
          break;
        
        case SDL_MOUSEMOTION:
          if(SDL_GetRelativeMouseMode()) {
            inputs.mouse_X = event.motion.xrel;
            inputs.mouse_Y = event.motion.yrel;
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
  gameLoop
  Does all the raycasting stuff, moves the player around, etc.
  If by chance the game ends, it returns false - which should
  terminate the main while loop and end the program.
*/
int gameLoop(
  int BUFFER_W,
  int BUFFER_H,
  int BUFFER_SCALE,
  unsigned int seed,
  Inputs *inputs,
  World *world,
  SDL_Renderer *renderer,
  SDL_Window   *window
) {
  // We dont want to have to pass all of these by reference, so
  // have all of them as static variables
  static float  cameraAngle_H_abs = 0.0,
                cameraAngle_V_abs = 0.0,
                f9,
                f10,
                f11,
                f12,
                playerSpeedLR,
                playerSpeedFB,
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
                f36,
                timeCoef;
  
  static long   l, gameTime;
  
  static int    BUFFER_HALF_W,
                BUFFER_HALF_H,
                k,
                m,
                i,
                blockSelected = 0,
                selectedPass,
                i6,
                i7,
                pixelX,
                i10,
                pixelY,
                i12,
                i13,
                i14,
                i15,
                finalPixelColor,
                pixelMist,
                pixelShade,
                blockFace,
                i25,
                pixelColor,
                
                /*
                  0: Gameplay
                  1: Pause menu
                  2: In-game options menu
                  3: Inventory
                */
                gamePopup,
                
                hotbarSelect,
                guiOn        = 1,
                debugOn      = 0,
                fogLog       = 0,
                drawDistance = 20,
                trapMouse    = 0,
                
                fps_lastmil  = 0,
                fps_count    = 0,
                fps_now      = 0;
  
  static char drawDistanceText [] = "Draw distance: 20\0";
  static char trapMouseText    [] = "Capture mouse: OFF";
  
  static char debugText        [][16] = {
    "M4KS 0.5",
    "X: ",
    "Y: ",
    "Z: ",
    "FPS: ",
    "ChunkX: ",
    "ChunkY: ",
    "ChunkZ: ",
  };
  
  static double d;
  
  static SDL_Rect backgroundRect;
  static SDL_Rect hotbarRect;
  static SDL_Rect hotbarSelectRect;
  
  static Inventory inventory;
  static Coords blockSelect       = {0};
  static Coords blockSelectOffset = {0};
  static Coords coordPass         = {0};
  static Coords blockRayPosition  = {0};

  static Coords playerPosition = {96.5, 65.0, 96.5};
  static Coords playerMovement = { 0.0,  0.0,  0.0};
  
  static int init = 1;
  if(init) {
    BUFFER_HALF_W = BUFFER_W / 2;
    BUFFER_HALF_H = BUFFER_H / 2;
    
    l = SDL_GetTicks();
    gameTime = 2048;
    
    gamePopup = 0;
    hotbarSelect = 0;
    
    backgroundRect.x = 0;
    backgroundRect.y = 0;
    backgroundRect.w = BUFFER_W;
    backgroundRect.h = BUFFER_H;
    
    hotbarRect.x = BUFFER_HALF_W - 77;
    hotbarRect.y = BUFFER_H - 18;
    hotbarRect.w = 154;
    hotbarRect.h = 18;
    
    hotbarSelectRect.y = hotbarRect.y;
    hotbarSelectRect.w = 18;
    hotbarSelectRect.h = 18;
    
    inventory.hotbar[0].blockid = 1;
    inventory.hotbar[1].blockid = 2;
    inventory.hotbar[2].blockid = 4;
    inventory.hotbar[3].blockid = 5;
    inventory.hotbar[4].blockid = 7;
    inventory.hotbar[5].blockid = 8;
    inventory.hotbar[6].blockid = 9;
    inventory.hotbar[7].blockid = 10;
    inventory.hotbar[8].blockid = 11;
    
    inventory.hotbar[0].amount  = 63;
    inventory.hotbar[1].amount  = 63;
    inventory.hotbar[2].amount  = 63;
    inventory.hotbar[3].amount  = 63;
    inventory.hotbar[4].amount  = 63;
    inventory.hotbar[5].amount  = 63;
    inventory.hotbar[6].amount  = 63;
    inventory.hotbar[7].amount  = 63;
    inventory.hotbar[8].amount  = 63;
  }
  
  f9  = sin(cameraAngle_H_abs),
  f10 = cos(cameraAngle_H_abs),
  f11 = sin(cameraAngle_V_abs),
  f12 = cos(cameraAngle_V_abs);
  
  // Skybox, basically
  timeCoef  = (float)(gameTime % 102944) / 16384;
  timeCoef  = sin(timeCoef);
  timeCoef /= sqrt(timeCoef * timeCoef + (1.0 / 128.0));
  timeCoef  = (timeCoef + 1) / 2;
  
  SDL_SetRenderDrawColor(
    renderer,
    153 * timeCoef,
    204 * timeCoef,
    255 * timeCoef,
    255
  );
  
  SDL_RenderClear(renderer);
  
  if(inputs->keyboard_Esc) {
    gamePopup = gamePopup ? 0 : 1;
    inputs->keyboard_Esc = 0;
  }
  
  fps_count++;
  if(fps_lastmil < SDL_GetTicks() - 1000) {
    fps_lastmil = SDL_GetTicks();
    fps_now     = fps_count;
    fps_count   = 0;
  }
  
  // Things that should run at a constant speed, regardless of
  // CPU power. If the rendering takes a long time, this will
  // fire more times to compensate.
  while(SDL_GetTicks() - l > 10L) {
    gameTime++;
    l += 10L;
    if(!gamePopup) {
      // Scroll wheel
      if(inputs->mouse_Wheel != 0) {
        hotbarSelect -= inputs->mouse_Wheel;
        hotbarSelect = nmod(hotbarSelect, 9);
        inputs->mouse_Wheel = 0;
      }
      
      // Looking around
      if(trapMouse) {
        f16 = (float)inputs->mouse_X * 1.5;
        f17 = (float)inputs->mouse_Y * 1.5;
      } else {
        f16 =
          (inputs->mouse_X - BUFFER_W * 2) /
          (float)BUFFER_W * 2.0;
        f17 =
          (inputs->mouse_Y - BUFFER_H * 2) /
          (float)BUFFER_H * 2.0;
      }
      
      f15 = sqrt(f16 * f16 + f17 * f17) - 1.2;
      if (f15 < 0.0)
        f15 = 0.0;
      if (f15 > 0.0) {
        cameraAngle_H_abs += f16 * f15 / 400.0;
        cameraAngle_V_abs -= f17 * f15 / 400.0;

        // Restrict camera vertical position
        if (cameraAngle_V_abs < -1.57)
          cameraAngle_V_abs = -1.57;
        if (cameraAngle_V_abs >  1.57)
          cameraAngle_V_abs =  1.57;
      }

      playerSpeedFB =
        (inputs->keyboard_W - inputs->keyboard_S) * 0.02;
      playerSpeedLR =
        (inputs->keyboard_D - inputs->keyboard_A) * 0.02;
    }
    
    // Moving around
    playerMovement.x *= 0.5;
    playerMovement.y *= 0.99;
    playerMovement.z *= 0.5;

    playerMovement.x +=
      f9  * playerSpeedFB + f10 * playerSpeedLR;
    playerMovement.z +=
      f10 * playerSpeedFB - f9  * playerSpeedLR;
    playerMovement.y += 0.003;
    
    
    // TODO: update this to check for collisions properly
    for (m = 0; m < 3; m++) {
      f16 =
        playerPosition.x + playerMovement.x * ((m + 2) % 3 / 2);
      f17 =
        playerPosition.y + playerMovement.y * ((m + 1) % 3 / 2);
      f19 =
        playerPosition.z + playerMovement.z * ((m + 2) % 3 / 2);

      for (i12 = 0; i12 < 12; i12++) {
        i13 = (int) (f16 + (i12 >> 0 & 0x1) * 0.6 - 0.3)  - 64;
        i14 = (int) (f17 + ((i12 >> 2) - 1) * 0.8 + 0.65) - 64;
        i15 = (int) (f19 + (i12 >> 1 & 0x1) * 0.6 - 0.3)  - 64;

        if (getBlock(world, i13, i14, i15) > 0) {
          if (m != 1) {
            goto label208;
          }
          if (
            inputs->keyboard_Space > 0 &&
            (playerMovement.y > 0.0)   &!
            gamePopup
          ) {
            inputs->keyboard_Space = 0;
            playerMovement.y = -0.1;
            goto label208;
          } 
          playerMovement.y = 0.0;
          goto label208;
        }
      }

      playerPosition.x = f16;
      playerPosition.y = f17;
      playerPosition.z = f19;
    }
    label208:;
  }
  
  i6 = 0;
  i7 = 0;
  if(!gamePopup) {
    if (inputs->mouse_Left > 0 && blockSelected) {
      setBlock(
        world,
        blockSelect.x,
        blockSelect.y,
        blockSelect.z, 0, 1
      );
      inputs->mouse_Left = 0;
    } 
    if (inputs->mouse_Right > 0 && blockSelected) {
      setBlock(
        world,
        blockSelect.x + blockSelectOffset.x,
        blockSelect.y + blockSelectOffset.y,
        blockSelect.z + blockSelectOffset.z,
        inventory.hotbar[hotbarSelect].blockid, 1
      );
      inputs->mouse_Right = 0;
    }
    if(inputs->keyboard_F1) {
      inputs->keyboard_F1 = 0;
      guiOn ^= 1;
    }
    if(inputs->keyboard_F3) {
      inputs->keyboard_F3 = 0;
      debugOn = 1 - debugOn;
    }
  }
  for (k = 0; k < 12; k++) {
    m =
      (int)
        (playerPosition.x + (k >> 0 & 0x1) * 0.6 - 0.3) - 64;
    i10 =
      (int)
        (playerPosition.y + ((k >> 2) - 1) * 0.8 + 0.65) - 64;
    pixelY =
      (int)
        (playerPosition.z + (k >> 1 & 0x1) * 0.6 - 0.3) - 64;
    if (
         m      >= 0
      && i10    >= 0
      && pixelY >= 0
      && m      < 64
      && i10    < 64
      && pixelY < 64
    ) {
      // TODO: Check if block is inside player before placing,
      // NOT THIS LOL
      setBlock(world, m, i10, pixelY, 0, 1);
    }
  }
  
  // Cast rays
  // selectedPass passes wether or not a block is selected to
  // the blockSelected variable
  selectedPass = 0;
  for (pixelX = 0; pixelX < BUFFER_W; pixelX++) {
    f18 = (pixelX - 107) / 90.0;
    for (pixelY = 0; pixelY < BUFFER_H; pixelY++) {
      f20 = (pixelY - 60) / 90.0;
      f21 = 1.0;
      f22 = f21 * f12 + f20 * f11;
      f23 = f20 * f12 - f21 * f11;
      f24 = f18 * f10 + f22 * f9;
      f25 = f22 * f10 - f18 * f9;
      finalPixelColor = 0;
      pixelMist = 255;
      d = drawDistance;
      f26 = 5.0;
      for (blockFace = 0; blockFace < 3; blockFace++) {
        f27 = f24;
        if (blockFace == 1)
          f27 = f23; 
        if (blockFace == 2)
          f27 = f25; 
        f28 = 1.0 / ((f27 < 0.0) ? -f27 : f27);
        f29 = f24 * f28;
        f30 = f23 * f28;
        f31 = f25 * f28;
        f32 = playerPosition.x - (int)playerPosition.x;
        if (blockFace == 1)
          f32 = playerPosition.y - (int)playerPosition.y;
        if (blockFace == 2)
          f32 = playerPosition.z - (int)playerPosition.z;
        if (f27 > 0.0)
          f32 = 1.0 - f32; 
        f33 = f28 * f32;
        f34 = playerPosition.x + f29 * f32;
        f35 = playerPosition.y + f30 * f32;
        f36 = playerPosition.z + f31 * f32;
        if (f27 < 0.0) {
          if (blockFace == 0)
            f34--; 
          if (blockFace == 1)
            f35--; 
          if (blockFace == 2)
            f36--; 
        } 
        while (f33 < d) {
          blockRayPosition.x = (int)f34 - 64;
          blockRayPosition.y = (int)f35 - 64;
          blockRayPosition.z = (int)f36 - 64;
          
          i25 = getBlock(
            world,
            blockRayPosition.x,
            blockRayPosition.y,
            blockRayPosition.z
          );
          if (i25 > 0) {
            i6 = (int)((f34 + f36) * 16.0) & 0xF;
            i7 = ((int)(f35 * 16.0) & 0xF) + 16;
            if (blockFace == 1) {
              i6 = (int)(f34 * 16.0) & 0xF;
              i7 = (int)(f36 * 16.0) & 0xF;
              if (f30 < 0.0)
                i7 += 32; 
            }
            // Block outline color
            pixelColor = 0xFFFFFF;
            if (
                (
                  !blockSelected                      ||
                  blockRayPosition.x != blockSelect.x ||
                  blockRayPosition.y != blockSelect.y ||
                  blockRayPosition.z != blockSelect.z
                ) || (
                   i6 > 0  
                && i7 % 16 > 0
                && i6 < 15
                && i7 % 16 < 15
              )
            ) {
              pixelColor = textures[
                i6 + i7 * 16 + i25 * 256 * 3
              ]; 
            }
            // See if the block is selected
            // There must be a better way to do this check...
            if (
              f33 < f26
              && (
                (
                   ! trapMouse
                  && pixelX == inputs->mouse_X / BUFFER_SCALE
                  && pixelY == inputs->mouse_Y / BUFFER_SCALE
                ) || (
                     trapMouse
                  && pixelX == BUFFER_HALF_W
                  && pixelY == BUFFER_HALF_H
                )
              )
            ) {
              selectedPass = 1;
              memcpy(
                &coordPass,
                &blockRayPosition,
                sizeof(Coords)
              );
              
              // Treating a coords set as an array and blockFace
              // as an index.
              blockSelectOffset.x = 0;
              blockSelectOffset.y = 0;
              blockSelectOffset.z = 0;
              *(&(blockSelectOffset.x) + blockFace)
                = 1 - 2 * (f27 > 0.0);
              
              f26 = f33;
            } 
            if (pixelColor > 0) {
              finalPixelColor = pixelColor;
              pixelMist =
                255 - (int)(f33 / (float)drawDistance * 255.0F);
              pixelShade =
                255 - (blockFace + 2) % 3 * 50;
              d = f33;
            } 
          } 
          f34 += f29;
          f35 += f30;
          f36 += f31;
          f33 += f28;
        }
      }
      
      // Draw inverted color crosshair
      if(trapMouse && (
        (pixelX == BUFFER_HALF_W
          && abs(BUFFER_HALF_H - pixelY) < 4) ||
        (pixelY == BUFFER_HALF_H
          && abs(BUFFER_HALF_W - pixelX) < 4)
      )) {
        finalPixelColor = 16777216 - finalPixelColor;
      }
      
      if(finalPixelColor > 0) {
        SDL_SetRenderDrawColor(
          renderer,
          (finalPixelColor >> 16 & 0xFF) * pixelShade / 255,
          (finalPixelColor >> 8  & 0xFF) * pixelShade / 255,
          (finalPixelColor       & 0xFF) * pixelShade / 255,
          fogLog ? sqrt(pixelMist) * 16 : pixelMist
        );
        
        SDL_RenderDrawPoint(renderer, pixelX, pixelY);
      }
      
    }
  }
  init = 0;
  
  // Pass info about selected block on
  blockSelected = selectedPass;
  memcpy(&blockSelect, &coordPass, sizeof(Coords));
  
  inputs->mouse_X /= BUFFER_SCALE;
  inputs->mouse_Y /= BUFFER_SCALE;
  
  // In-game menus
  if(gamePopup) {
    SDL_SetRelativeMouseMode(0);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_RenderFillRect(renderer, &backgroundRect);
    
    switch(gamePopup) {
      // Pause menu
      case 1:
        if(button(renderer, "Resume",
          BUFFER_HALF_W - 64, 20, 128,
          inputs->mouse_X, inputs->mouse_Y) && inputs->mouse_Left
        ) {
          gamePopup = 0;
        }

        if(button(renderer, "Options",
          BUFFER_HALF_W - 64, 42, 128,
          inputs->mouse_X, inputs->mouse_Y) && inputs->mouse_Left
        ) {
          gamePopup = 2;
        }

        if(button(renderer, "Exit",
          BUFFER_HALF_W - 64, 64, 128,
          inputs->mouse_X, inputs->mouse_Y) && inputs->mouse_Left
        ) {
          return 0;
        }
        break;

      // Options
      case 2:
        if(button(renderer, drawDistanceText,
          BUFFER_HALF_W - 64, 20, 128,
          inputs->mouse_X, inputs->mouse_Y) && inputs->mouse_Left
        ) {
          switch(drawDistance) {
            case 20:
              drawDistance = 32;
              break;
            case 32:
              drawDistance = 64;
              break;
            case 64:
              drawDistance = 96;
              break;
            case 96:
              drawDistance = 128;
              break;
            default:
              drawDistance = 20;
              break;
          }
          strnum(drawDistanceText, 15, drawDistance);
        }
        
        if(button(renderer, trapMouseText,
          BUFFER_HALF_W - 64, 42, 128,
          inputs->mouse_X, inputs->mouse_Y) && inputs->mouse_Left
        ) {
          if(trapMouse) {
            trapMouse = 0;
            sprintf(trapMouseText + 15, "OFF");
          } else {
            trapMouse = 1;
            sprintf(trapMouseText + 15, "ON");
          }
        }
        
        if(button(renderer, "Back",
          BUFFER_HALF_W - 64, 64, 128,
          inputs->mouse_X, inputs->mouse_Y) && inputs->mouse_Left
        ) {
          gamePopup = 1;
        }
        break;
    }
  } else {
    if(trapMouse) {
      SDL_SetRelativeMouseMode(1);
    }
    if(guiOn) {
      // Debug screen
      if(debugOn) {
        // Coordinates
        strnum(debugText[1], 3, (int)playerPosition.x - 64);
        strnum(debugText[2], 3, (int)playerPosition.y - 64);
        strnum(debugText[3], 3, (int)playerPosition.z - 64);
        
        // FPS
        strnum(debugText[4], 5, fps_now);
        
        // Chunk coordinates
        strnum(
          debugText[5], 8, 
          ((int)playerPosition.x - 64) >> 6
        );
        strnum(
          debugText[6], 8, 
          ((int)playerPosition.y - 64) >> 6
        );
        strnum(
          debugText[7], 8, 
          ((int)playerPosition.z - 64) >> 6
        );
        
        // Shadow
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        for(i = 0; i < 8; i++)
          drawStr(renderer, debugText[i], 3, i * 8 + 3);
        
        // Text
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for(i = 0; i < 8; i++)
          drawStr(renderer, debugText[i], 2, i * 8 + 2);
      }
      // Hotbar
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
      SDL_RenderFillRect(renderer, &hotbarRect);
      
      hotbarSelectRect.x =
        BUFFER_HALF_W - 77 + hotbarSelect * 17;
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderDrawRect(renderer, &hotbarSelectRect);
      
      for(i = 0; i < 9; i++)
        drawSlot(
          renderer,
          &inventory.hotbar[i], 
          BUFFER_HALF_W - 76 + i * 17,
          BUFFER_H - 17,
          inputs->mouse_X,
          inputs->mouse_Y
        );
    }
  }
  
  if(inputs->mouse_Left) inputs->mouse_Left = 0;
  if(inputs->mouse_X)    inputs->mouse_X    = 0;
  
  return 1;
}
