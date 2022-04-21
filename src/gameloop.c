#include "gameloop.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * WARNING!!!                                                                *
 *                                                                           *
 * This file is where all the decompiled nonsense is. Things are so heavily  *
 * nested and spaghetti-like that its the only place with an indent size of  *
 * two spaces.                                                               *
 *                                                                           *
 * Looking at this code for extended periods of time can cause adverse       *
 * psychological effects.                                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* gameLoop
 * Does all the raycasting stuff, moves the player around, etc.
 * If by chance the game ends, it returns false - which should
 * terminate the main while loop and end the program.
 */
int gameLoop (
        unsigned int seed,
        Inputs *inputs,
        World *world,
        SDL_Renderer *renderer
) {
  // We dont want to have to pass all of these by reference, so
  // have all of them as static variables
  static float  cameraAngle_H = 0.0,
                cameraAngle_V = 0.0,
                f9,
                f10,
                f11,
                f12,
                playerSpeedLR,
                playerSpeedFB,
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
  
  static int    m,
                blockSelected = 0,
                selectedPass,
                i6,
                i7,
                pixelX,
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
                  1: Main menu
                  2: World select
                  3: World creation
                  4: Loading
                  5: Gameplay
                  6: World editing (renaming etc)
                  7: Join game
                  8: Options
                */
                gameState = 0,
                
                /*
                  0: Gameplay
                  1: Pause menu
                  2: In-game options menu
                  3: Inventory
                  4: Advanced debug menu
                  5: Chunk peek
                  6: Chat
                */
                gamePopup,
                
                guiOn        = 1,
                debugOn      = 0,
                fogLog       = 0,
                drawDistance = 20,
                trapMouse    = 0;

  static u_int32_t fps_lastmil  = 0,
                   fps_count    = 0,
                   fps_now      = 0;
  
  static double d;
  
  static SDL_Rect backgroundRect;

  static Player player = {0};
  
  static IntCoords blockSelect       = {0};
  static IntCoords blockSelectOffset = {0};
  static IntCoords coordPass         = {0};
  static IntCoords blockRayPosition  = {0};

  static Coords playerMovement = {0.0, 0.0, 0.0};
  
  static Chunk *chunk;
  
  static int init = 0;
  
  if (init) {
    l = SDL_GetTicks();
    gameTime = 2048;
    
    player.pos.x = 96.5;
    player.pos.y = 65.0;
    player.pos.z = 96.5;
    
    cameraAngle_H = 0.0;
    cameraAngle_V = 0.0;
    
    gamePopup = 0;
    
    backgroundRect.x = 0;
    backgroundRect.y = 0;
    backgroundRect.w = BUFFER_W;
    backgroundRect.h = BUFFER_H;
    
    player.inventory.hotbar[0].blockid = 0;
    player.inventory.hotbar[1].blockid = 0;
    player.inventory.hotbar[2].blockid = 0;
    player.inventory.hotbar[3].blockid = 0;
    player.inventory.hotbar[4].blockid = 0;
    player.inventory.hotbar[5].blockid = 0;
    player.inventory.hotbar[6].blockid = 0;
    player.inventory.hotbar[7].blockid = 0;
    player.inventory.hotbar[8].blockid = 0;
    
    player.inventory.hotbar[0].amount  = 0;
    player.inventory.hotbar[1].amount  = 0;
    player.inventory.hotbar[2].amount  = 0;
    player.inventory.hotbar[3].amount  = 0;
    player.inventory.hotbar[4].amount  = 0;
    player.inventory.hotbar[5].amount  = 0;
    player.inventory.hotbar[6].amount  = 0;
    player.inventory.hotbar[7].amount  = 0;
    player.inventory.hotbar[8].amount  = 0;

    player.inventory.hotbarSelect = 0;
    
    chatAdd("Game started");
    
    init = 0;
  }
  
  switch (gameState) {
    // A main menu
    case 0:
      if (state_title(renderer, inputs, &gameState, &init)) return 0;
      break;
    
    // Generate a world and present a loading screen
    case 4:
      if (state_loading(renderer, world, seed, player.pos)) gameState = 5;
      break;
    
    // The actual gameplay
    case 5:
      /* Look to see if there are chunks that need to be
      loaded in*/
      /*
      if(chunkLoadNum < CHUNKARR_SIZE) {
        chunkLoadCoords.x =
          ((chunkLoadNum % CHUNKARR_DIAM) -
          CHUNKARR_RAD) * 64 + player.pos.x - 64;
        chunkLoadCoords.y =
          (((chunkLoadNum / CHUNKARR_DIAM) % CHUNKARR_DIAM) - 
          CHUNKARR_RAD) * 64 + player.pos.y - 64;
        chunkLoadCoords.z =
          ((chunkLoadNum / (CHUNKARR_DIAM * CHUNKARR_DIAM)) -
          CHUNKARR_RAD) * 64 + player.pos.z - 64;
        chunkLoadNum++;
        
        genChunk(
          world, seed,
          chunkLoadCoords.x,
          chunkLoadCoords.y,
          chunkLoadCoords.z, 1, 0,
          player.pos
        );
      } else {
        chunkLoadNum = 0;
      }
      */

      f9  = sin(cameraAngle_H),
      f10 = cos(cameraAngle_H),
      f11 = sin(cameraAngle_V),
      f12 = cos(cameraAngle_V);
      
      // Skybox, basically
      timeCoef  = (float)(gameTime % 102944) / 16384;
      timeCoef  = sin(timeCoef);
      timeCoef /= sqrt(timeCoef * timeCoef + (1.0 / 128.0));
      timeCoef  = (timeCoef + 1) / 2;
      
      SDL_SetRenderDrawColor (
        renderer,
        153 * timeCoef,
        204 * timeCoef,
        255 * timeCoef,
        255
      );
      
      SDL_RenderClear(renderer);
      
      if (inputs->keyboard_Esc) {
        gamePopup = gamePopup ? 0 : 1;
        inputs->keyboard_Esc = 0;
      }
      
      fps_count++;
      if (fps_lastmil < SDL_GetTicks() - 1000) {
        fps_lastmil = SDL_GetTicks();
        fps_now     = fps_count;
        fps_count   = 0;
      }
      
      /* Things that should run at a constant speed, regardless
      of CPU power. If the rendering takes a long time, this
      will fire more times to compensate. */
      while (SDL_GetTicks() - l > 10L) {
        gameTime++;
        l += 10L;
        if (!gamePopup) {
          // Scroll wheel
          if(inputs->mouse_Wheel != 0) {
            player.inventory.hotbarSelect -= inputs->mouse_Wheel;
            player.inventory.hotbarSelect = nmod (
              player.inventory.hotbarSelect, 9
            );
            inputs->mouse_Wheel = 0;
          }
          
          // Looking around
          if (trapMouse) {
            cameraAngle_H += (float)inputs->mouse_X / 64;
            cameraAngle_V -= (float)inputs->mouse_Y / 64;
          } else {
            f16 =
              (inputs->mouse_X - BUFFER_W * 2) /
              (float)BUFFER_W * 2.0;

            f17 =
              (inputs->mouse_Y - BUFFER_H * 2) /
              (float)BUFFER_H * 2.0;
          
            float cameraMoveDistance =
              sqrt(f16 * f16 + f17 * f17) - 1.2;

            if (cameraMoveDistance < 0.0)
              cameraMoveDistance = 0.0;
            if (cameraMoveDistance > 0.0) {
              cameraAngle_H +=
                f16 * cameraMoveDistance / 400.0;
              cameraAngle_V -=
                f17 * cameraMoveDistance / 400.0;
            }
          }

          // Restrict camera vertical position
          if (cameraAngle_V < -1.57) cameraAngle_V = -1.57;
          if (cameraAngle_V >  1.57) cameraAngle_V =  1.57;

          playerSpeedFB = (inputs->keyboard_W - inputs->keyboard_S) * 0.02;
          playerSpeedLR = (inputs->keyboard_D - inputs->keyboard_A) * 0.02;
        } else {
          playerSpeedFB = 0;
          playerSpeedLR = 0;
        }
        
        // Moving around
        playerMovement.x *= 0.5;
        playerMovement.y *= 0.99;
        playerMovement.z *= 0.5;

        playerMovement.x += f9  * playerSpeedFB + f10 * playerSpeedLR;
        playerMovement.z += f10 * playerSpeedFB - f9  * playerSpeedLR;
        playerMovement.y += 0.003;
        
        for (m = 0; m < 3; m++) {
          f16 =
            player.pos.x +
            playerMovement.x * ((m + 2) % 3 / 2);
          f17 =
            player.pos.y +
            playerMovement.y * ((m + 1) % 3 / 2);
          f19 =
            player.pos.z +
            playerMovement.z * ((m + 3) % 3 / 2);
          
          for (i12 = 0; i12 < 12; i12++) {
            i13 = (int)(f16 + (i12 >> 0 & 0x1) * 0.6 - 0.3)  - 64;
            i14 = (int)(f17 + ((i12 >> 2) - 1) * 0.8 + 0.65) - 64;
            i15 = (int)(f19 + (i12 >> 1 & 0x1) * 0.6 - 0.3)  - 64;
            
            if (World_getBlock(world, i13, i14, i15) > 0) {
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

          player.pos.x = f16;
          player.pos.y = f17;
          player.pos.z = f19;
          
          label208:;
        }
      }
      
      i6 = 0;
      i7 = 0;
      
      if (!gamePopup) {

        if (blockSelected) {
          InvSlot *activeSlot = &player.inventory.hotbar [
            player.inventory.hotbarSelect
          ];

          // Breaking blocks
          if (inputs->mouse_Left > 0) {
            InvSlot pickedUp = {
              .blockid = World_getBlock (
                world,
                blockSelect.x,
                blockSelect.y,
                blockSelect.z
              ),
              .amount     = 1,
              .durability = 1
            };

            Inventory_transferIn(&player.inventory, &pickedUp);
            
            World_setBlock (
              world,
              blockSelect.x,
              blockSelect.y,
              blockSelect.z, 0, 1
            );
          }
          
          blockSelectOffset.x += blockSelect.x;
          blockSelectOffset.y += blockSelect.y;
          blockSelectOffset.z += blockSelect.z;

          // Placing blocks
          if (inputs->mouse_Right > 0) {
            if (
              // Player cannot be obstructing the block
              (
                fabs(player.pos.x - 64.5 - blockSelectOffset.x) >= 0.8 ||
                fabs(player.pos.y - 64 - blockSelectOffset.y) >= 1.45 ||
                fabs(player.pos.z - 64.5 - blockSelectOffset.z) >= 0.8
              ) &&
              // Player must have enough of that block
              activeSlot->amount > 0
            ) {
              int blockSet = World_setBlock (
                world,
                blockSelectOffset.x,
                blockSelectOffset.y,
                blockSelectOffset.z,
                activeSlot->blockid, 1
              );

              if (blockSet) {
                activeSlot->amount --;
                if (activeSlot->amount == 0) activeSlot->blockid = 0;
              }
            }
          }
        }

        inputs->mouse_Left = 0;
        inputs->mouse_Right = 0;

        // Toggle GUI
        if (inputs->keyboard_F1) {
          inputs->keyboard_F1 = 0;
          guiOn ^= 1;
        }

        // Toggle debug mode
        if (inputs->keyboard_F3) {
          inputs->keyboard_F3 = 0;
          debugOn = !debugOn;
        }

        // Enter chat
        if (inputs->keyboard_T) {
          inputs->keyboard_T = 0;
          inputs->keyTyped   = 0;
          gamePopup = 6;
        }

        // Enter inventory
        if (inputs->keyboard_E) {
          inputs->keyboard_E = 0;
          inputs->keyTyped   = 0;
          gamePopup = 3;
        }
      }

      #ifndef small
      if (inputs->keyboard_F4) {
        inputs->keyboard_F4 = 0;
        gamePopup = (gamePopup == 4) ? 0 : 4;
      }
      #endif
      
      /* Cast rays. selectedPass passes wether or not a block is
      selected to the blockSelected variable */
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
            f32 = player.pos.x - (int)player.pos.x;
            if (blockFace == 1)
              f32 = player.pos.y - (int)player.pos.y;
            if (blockFace == 2)
              f32 = player.pos.z - (int)player.pos.z;
            if (f27 > 0.0)
              f32 = 1.0 - f32; 
            f33 = f28 * f32;
            f34 = player.pos.x + f29 * f32;
            f35 = player.pos.y + f30 * f32;
            f36 = player.pos.z + f31 * f32;
            if (f27 < 0.0) {
              if (blockFace == 0)
                f34--; 
              if (blockFace == 1)
                f35--; 
              if (blockFace == 2)
                f36--; 
            }
            
            /* Whatever's in this loop needs to run *extremely*
            fast */
            while (f33 < d) {
              blockRayPosition.x = (int)f34 - 64;
              blockRayPosition.y = (int)f35 - 64;
              blockRayPosition.z = (int)f36 - 64;
              
              /* Imitate getBlock so we don't have to launch
              into a function then another function a zillion
              times per second. */
              static IntCoords lookup_ago = {
                100000000,
                100000000,
                100000000
              }, lookup_now;
              
              lookup_now.x = blockRayPosition.x >> 6;
              lookup_now.y = blockRayPosition.y >> 6;
              lookup_now.z = blockRayPosition.z >> 6;
              
              if (
                lookup_now.x != lookup_ago.x ||
                lookup_now.y != lookup_ago.y ||
                lookup_now.z != lookup_ago.z
              ) {
                lookup_ago = lookup_now;
                
                lookup_now.x &= 0b1111111111;
                lookup_now.y &= 0b1111111111;
                lookup_now.z &= 0b1111111111;
                
                lookup_now.y <<= 10;
                lookup_now.z <<= 20;
                
                int lookup_hash =
                  lookup_now.x | lookup_now.y | lookup_now.z;
                lookup_hash++;
                
                int lookup_first  = 0,
                    lookup_last   = CHUNKARR_SIZE - 1,
                    lookup_middle = (CHUNKARR_SIZE - 1) / 2;
                
                while (lookup_first <= lookup_last) {
                  if (
                    world->chunk[lookup_middle].coordHash
                    > lookup_hash
                  ) lookup_first = lookup_middle + 1;
                  else if (
                    world->chunk[lookup_middle].coordHash
                    == lookup_hash
                  ) {
                    chunk = &world->chunk[lookup_middle];
                    goto foundChunk;
                  } else lookup_last = lookup_middle - 1;
                  lookup_middle =
                    (lookup_first + lookup_last) / 2;
                }
                chunk = NULL;
              }
              /*
              chunk = chunkLookup(
                world,
                blockRayPosition.x,
                blockRayPosition.y,
                blockRayPosition.z
              );
              */
              foundChunk: if (chunk) {
                i25 = chunk->blocks[
                   nmod(blockRayPosition.x, 64)        +
                  (nmod(blockRayPosition.y, 64) << 6 ) +
                  (nmod(blockRayPosition.z, 64) << 12)
                ];
              } else {
                i25 = 0;
                goto chunkNull;
              }
              
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
                  ) || !guiOn || gamePopup
                ) {
                  pixelColor = textures[i6 + (i7 << 4) + i25 * 256 * 3];
                }
                
                /* See if the block is selected. There must be a
                better way to do this check... */
                if (
                  f33 < f26 && (
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
                  coordPass = blockRayPosition;
                  
                  /* Treating a coords set as an array and
                  blockFace as an index. */
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
                    255 - (int)
                    (f33 / (float)drawDistance * 255.0F);
                  pixelShade =
                    255 - (blockFace + 2) % 3 * 50;
                  d = f33;
                } 
              }
              chunkNull:
              f34 += f29;
              f35 += f30;
              f36 += f31;
              f33 += f28;
            } // This concludes our warpspeed rampage
          }
          
          // Draw inverted color crosshair
          if (trapMouse && (
            (pixelX == BUFFER_HALF_W
              && abs(BUFFER_HALF_H - pixelY) < 4) ||
            (pixelY == BUFFER_HALF_H
              && abs(BUFFER_HALF_W - pixelX) < 4)
          )) {
            finalPixelColor = 16777216 - finalPixelColor;
          }
          
          if (finalPixelColor > 0) {
            SDL_SetRenderDrawColor(
              renderer,
              ((finalPixelColor >> 16 & 0xFF) * pixelShade) >> 8,
              ((finalPixelColor >> 8  & 0xFF) * pixelShade) >> 8,
              ((finalPixelColor       & 0xFF) * pixelShade) >> 8,
              fogLog ? sqrt(pixelMist) * 16 : pixelMist
            );
            
            SDL_RenderDrawPoint(renderer, pixelX, pixelY);
          }
        }
      }
      
      // Pass info about selected block on
      blockSelected = selectedPass;
      blockSelect   = coordPass;
      
      inputs->mouse_X /= BUFFER_SCALE;
      inputs->mouse_Y /= BUFFER_SCALE;
      
      // If we need to take a screenshot, do so
      if (inputs->keyboard_F2) {
        inputs->keyboard_F2 = 0;
        screenshot(renderer);
      }
      
      // In-game menus

      if (gamePopup) {
        SDL_SetRelativeMouseMode(0);
      }
      
      switch (gamePopup) {
        // HUD
        case 0:
          if (trapMouse) SDL_SetRelativeMouseMode(1);
          if (guiOn) popup_hud (
            renderer, inputs, world,
            &debugOn, &fps_now, &player
          );
          break;
           
        // Pause menu
        case 1:
          tblack(renderer);
          SDL_RenderFillRect(renderer, &backgroundRect);
          popup_pause(renderer, inputs, &gamePopup, &gameState);
          break;
        
        // Options
        case 2:
          tblack(renderer);
          SDL_RenderFillRect(renderer, &backgroundRect);
          popup_options (
            renderer, inputs,
            &gamePopup, &drawDistance, &trapMouse
          );
          break;
        
        // Inventory
        case 3:
          popup_inventory(renderer, inputs, &player, &gamePopup);
          break;

        #ifndef small
        // Advanced debug menu
        case 4:
          tblack(renderer);
          SDL_RenderFillRect(renderer, &backgroundRect);
          popup_debugTools(renderer, inputs, &gamePopup);
          break;
        
        // Chunk peek
        case 5:
          tblack(renderer);
          SDL_RenderFillRect(renderer, &backgroundRect);
          popup_chunkPeek(renderer, inputs, world, &gamePopup, &player);
          break;
        #endif
        
        // Chat
        case 6:
          popup_chat(renderer, inputs, &gameTime);
          break;
      }

      // Clean up input struct
      inputs->keyTyped = 0;
      inputs->keySym   = 0;
      break;

    case 8:
      state_options(renderer, inputs, &gameState, &drawDistance, &trapMouse);
      break;
    
    default:
      state_egg(renderer, inputs, &gameState);
      break;
  }

  if (gameState != 5 || gamePopup) {
    inputs->mouse_Left  = 0;
    inputs->mouse_Right = 0;
  }
  
  return 1;
}

int screenshot (SDL_Renderer *renderer) {
        SDL_Surface *grab = SDL_CreateRGBSurfaceWithFormat (
                0, BUFFER_W * BUFFER_SCALE, BUFFER_H * BUFFER_SCALE,
                32, SDL_PIXELFORMAT_ARGB8888
        );

        SDL_RenderReadPixels (
                renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
                grab->pixels, grab->pitch
        );

        int saved = SDL_SaveBMP(grab, "screenshot.bmp"); 
        SDL_FreeSurface(grab);

        if (saved == 0) {
                chatAdd("Saved screenshot\n");
                return 0;
        } else {
                chatAdd("Couldn't save screenshot\n");
                return 1;
        }
}
