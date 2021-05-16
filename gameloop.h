/*
  gameLoop
  Does all the raycasting stuff, moves the player around, etc.
  If by chance the game ends, it returns false - which should
  terminate the main while loop and end the program.
*/
int gameLoop(
  unsigned int seed,
  Inputs *inputs,
  World *world,
  SDL_Renderer *renderer,
  SDL_Window   *window
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
                i,
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
                gameState    = 0,
                
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
                
                hotbarSelect,
                guiOn        = 1,
                debugOn      = 0,
                fogLog       = 0,
                drawDistance = 20,
                trapMouse    = 0,
                
                chatDrawIndex,
                
                fps_lastmil  = 0,
                fps_count    = 0,
                fps_now      = 0;
  
  static char drawDistanceText [] = "Draw distance: 20\0";
  static char trapMouseText    [] = "Capture mouse: OFF";
  
  static char debugText        [][16] = {
    "M4KC 0.7",
    "X: ",
    "Y: ",
    "Z: ",
    "FPS: ",
    "ChunkX: ",
    "ChunkY: ",
    "ChunkZ: ",
  };
  
  static char  chatHistory      [11][64] = {0};
  static int   chatHistoryFade  [11]     = {0};
  static int   chatHistoryIndex          = 0;
  static char  chatBox          [64]     = {0};
  static int   chatBoxCursor             = 0;
  
  static double d;
  
  static SDL_Rect backgroundRect;
  static SDL_Rect hotbarRect;
  static SDL_Rect hotbarSelectRect;
  static SDL_Rect chatBoxRect = {0, 0, 0, 9};
  
  static Inventory inventory;
  static IntCoords blockSelect       = {0};
  static IntCoords blockSelectOffset = {0};
  static IntCoords coordPass         = {0};
  static IntCoords blockRayPosition  = {0};

  static Coords playerPosition = {96.5, 65.0, 96.5};
  static Coords playerMovement = { 0.0,  0.0,  0.0};
  
  static Chunk *chunk;
  
  static int init = 0;
  if(init) {
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
    
    chatBoxRect.y = BUFFER_H - 9;
    chatBoxRect.w = BUFFER_W;
    
    chatAdd(
      chatHistory, chatHistoryFade, &chatHistoryIndex,
      "Game started"
    );
    
    init = 0;
  }
  
  switch(gameState) {
    case 0:
      inputs->mouse_X /= BUFFER_SCALE;
      inputs->mouse_Y /= BUFFER_SCALE;
      
      dirtBg(renderer);
      white(renderer);
      centerStr(
        renderer,
        "M4KC",
        BUFFER_HALF_W,
        20
      );
      
      if(button(renderer, "Singleplayer",
        BUFFER_HALF_W - 64, 42, 128,
        inputs->mouse_X, inputs->mouse_Y) &&
        inputs->mouse_Left
      ) {
        gameState = 5;
        init = 1;
      }
      
      if(button(renderer, "Quit Game",
        BUFFER_HALF_W - 64, 64, 128,
        inputs->mouse_X, inputs->mouse_Y) &&
        inputs->mouse_Left
      ) {
        return 0;
      }
      break;
    
    case 5:
      f9  = sin(cameraAngle_H),
      f10 = cos(cameraAngle_H),
      f11 = sin(cameraAngle_V),
      f12 = cos(cameraAngle_V);
      
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
      
      /* Things that should run at a constant speed, regardless
      of CPU power. If the rendering takes a long time, this
      will fire more times to compensate. */
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
            cameraAngle_H += (float)inputs->mouse_X / 64;
            cameraAngle_V = (float)inputs->mouse_Y / 64;
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
          if (cameraAngle_V < -1.57)
            cameraAngle_V = -1.57;
          if (cameraAngle_V >  1.57)
            cameraAngle_V =  1.57;

          playerSpeedFB =
            (inputs->keyboard_W - inputs->keyboard_S) * 0.02;
          playerSpeedLR =
            (inputs->keyboard_D - inputs->keyboard_A) * 0.02;
        } else {
          playerSpeedFB = 0;
          playerSpeedLR = 0;
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
            playerPosition.x +
            playerMovement.x * ((m + 2) % 3 / 2);
          f17 =
            playerPosition.y +
            playerMovement.y * ((m + 1) % 3 / 2);
          f19 =
            playerPosition.z +
            playerMovement.z * ((m + 2) % 3 / 2);
          
          for (i12 = 0; i12 < 12; i12++) {
            i13 = (int)
            (f16 + (i12 >> 0 & 0x1) * 0.6 - 0.3)  - 64;
            i14 = (int)
            (f17 + ((i12 >> 2) - 1) * 0.8 + 0.65) - 64;
            i15 = (int)
            (f19 + (i12 >> 1 & 0x1) * 0.6 - 0.3)  - 64;
            
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
        if(inputs->mouse_Left > 0 && blockSelected) {
          setBlock(
            world,
            blockSelect.x,
            blockSelect.y,
            blockSelect.z, 0, 1
          );
          inputs->mouse_Left = 0;
        }
        
        blockSelectOffset.x += blockSelect.x;
        blockSelectOffset.y += blockSelect.y;
        blockSelectOffset.z += blockSelect.z;
        if(inputs->mouse_Right > 0) {
          if(!(
            blockSelectOffset.x ==
              (int)playerPosition.x - 64 &&
            blockSelectOffset.z == 
              (int)playerPosition.z - 64 &&
            (
              blockSelectOffset.y ==
                (int)playerPosition.y - 64 ||
              blockSelectOffset.y ==
                (int)playerPosition.y - 63
            )
          )) {
            setBlock(
              world,
              blockSelectOffset.x,
              blockSelectOffset.y,
              blockSelectOffset.z,
              inventory.hotbar[hotbarSelect].blockid, 1
            );
          }
          inputs->mouse_Right = 0;
        }
        if(inputs->keyboard_F1) {
          inputs->keyboard_F1 = 0;
          guiOn ^= 1;
        }
        if(inputs->keyboard_F2) {
          inputs->keyboard_F2 = 0;
          // TODO: This segfaults. Fix
          /*
          SDL_Surface *grab = SDL_GetWindowSurface(window);
          if(SDL_SaveBMP(grab, "screenshot.bmp")) {
            printf("saved screenshot\n");
          } else {
            printf("couldn't save screenshot\n");
          }
          */
        }
        if(inputs->keyboard_F3) {
          inputs->keyboard_F3 = 0;
          debugOn = !debugOn;
        }
        if(inputs->keyboard_T) {
          inputs->keyboard_T = 0;
          inputs->keyTyped   = 0;
          gamePopup = 6;
        }
        
      }
      
      // Cast rays
      // selectedPass passes wether or not a block is selected
      // to  the blockSelected variable
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
            
            // Whatever's in this loop needs to run *extremely*
            // fast
            while (f33 < d) {
              blockRayPosition.x = (int)f34 - 64;
              blockRayPosition.y = (int)f35 - 64;
              blockRayPosition.z = (int)f36 - 64;
              
              // Imitate getBlock so we don't have to launch
              // into a function then another function a
              // zillion times per second.
              static IntCoords lookup_ago = {
                100000000,
                100000000,
                100000000
              }, lookup_now;
              
              lookup_now.x = blockRayPosition.x >> 6;
              lookup_now.y = blockRayPosition.y >> 6;
              lookup_now.z = blockRayPosition.z >> 6;
              
              if(
                lookup_now.x != lookup_ago.x ||
                lookup_now.y != lookup_ago.y ||
                lookup_now.z != lookup_ago.z
              ) {
                memcpy(
                  &lookup_ago,
                  &lookup_now,
                  sizeof(IntCoords)
                );
                
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
                
                while(lookup_first <= lookup_last) {
                  if(
                    world->chunk[lookup_middle].coordHash
                    > lookup_hash
                  ) lookup_first = lookup_middle + 1;
                  else if(
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
              foundChunk: if(chunk) {
                i25 = chunk->blocks[
                   nmod(blockRayPosition.x, 64)        +
                  (nmod(blockRayPosition.y, 64) << 6 ) +
                  (nmod(blockRayPosition.z, 64) << 12)
                ];
              } else {
                i25 = 0;
                goto chunkNull;
              }
              
              if(i25 > 0) {
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
                    i6 + (i7 << 4) + i25 * 256 * 3
                  ]; 
                }
                // See if the block is selected
                // There must be a better way to do this
                // check...
                if (
                  f33 < f26
                  && (
                    (
                       ! trapMouse
                      && pixelX == inputs->mouse_X /
                                   BUFFER_SCALE
                      && pixelY == inputs->mouse_Y /
                                   BUFFER_SCALE
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
                    sizeof(IntCoords)
                  );
                  
                  // Treating a coords set as an array and
                  //blockFace as an index.
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
              ((finalPixelColor >> 16 & 0xFF) * pixelShade)
              >> 8,
              ((finalPixelColor >> 8  & 0xFF) * pixelShade)
              >> 8,
              ((finalPixelColor       & 0xFF) * pixelShade)
              >> 8,
              fogLog ? sqrt(pixelMist) * 16 : pixelMist
            );
            
            SDL_RenderDrawPoint(renderer, pixelX, pixelY);
          }
          
        }
      }
      
      // Pass info about selected block on
      blockSelected = selectedPass;
      memcpy(&blockSelect, &coordPass, sizeof(IntCoords));
      
      inputs->mouse_X /= BUFFER_SCALE;
      inputs->mouse_Y /= BUFFER_SCALE;
      
      // In-game menus
      if(gamePopup) {
        SDL_SetRelativeMouseMode(0);
        
        if(gamePopup != 6) {
          tblack(renderer);
          SDL_RenderFillRect(renderer, &backgroundRect);
        }
        
        switch(gamePopup) {
          // Pause menu
          case 1:
            if(button(renderer, "Back to Game",
              BUFFER_HALF_W - 64, 20, 128,
              inputs->mouse_X, inputs->mouse_Y) &&
              inputs->mouse_Left
            ) {
              gamePopup = 0;
            }
            
            if(button(renderer, "Options...",
              BUFFER_HALF_W - 64, 42, 128,
              inputs->mouse_X, inputs->mouse_Y) &&
              inputs->mouse_Left
            ) {
              gamePopup = 2;
            }
            
            if(button(renderer, "Quit to Title",
              BUFFER_HALF_W - 64, 64, 128,
              inputs->mouse_X, inputs->mouse_Y) &&
              inputs->mouse_Left
            ) {
              gameState = 0;
            }
            break;
          
          // Options
          case 2:
            if(button(renderer, drawDistanceText,
              BUFFER_HALF_W - 64, 20, 128,
              inputs->mouse_X, inputs->mouse_Y) &&
              inputs->mouse_Left
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
              inputs->mouse_X, inputs->mouse_Y) &&
              inputs->mouse_Left
            ) {
              if(trapMouse) {
                trapMouse = 0;
                sprintf(trapMouseText + 15, "OFF");
              } else {
                trapMouse = 1;
                sprintf(trapMouseText + 15, "ON");
              }
            }
            
            if(button(renderer, "Debug...",
              BUFFER_HALF_W - 64, 64, 128,
              inputs->mouse_X, inputs->mouse_Y) &&
              inputs->mouse_Left
            ) {
              gamePopup = 4;
            }
            
            if(button(renderer, "Done",
              BUFFER_HALF_W - 64, 86, 128,
              inputs->mouse_X, inputs->mouse_Y) &&
              inputs->mouse_Left
            ) {
              gamePopup = 1;
            }
            break;
          
          // Inventory
          case 3:
            // TODO: draw inventory
            break;
          
          // Advanced debug menu
          case 4:
            if(button(renderer, "Chunk Peek",
              BUFFER_HALF_W - 64, 20, 128,
              inputs->mouse_X, inputs->mouse_Y) &&
              inputs->mouse_Left
            ) {
              gamePopup = 5;
            }
            
            if(button(renderer, "Done",
              BUFFER_HALF_W - 64, 42, 128,
              inputs->mouse_X, inputs->mouse_Y) &&
              inputs->mouse_Left
            ) {
              gamePopup = 2;
            }
            break;
          
          // Chunk peek
          case 5:
            ;
            static int chunkPeekRX,
                       chunkPeekRY,
                       chunkPeekRYMax = 0,
                       chunkPeekRZ,
                       chunkPeekColor;
            static Chunk *debugChunk;
            static char chunkPeekText[][32] = {
              "coordHash: ",
              "loaded: "
            };
            debugChunk = chunkLookup(
              world,
              (int)playerPosition.x - 64,
              (int)playerPosition.y - 64,
              (int)playerPosition.z - 64
            );
            white(renderer);
            if(debugChunk != NULL) {
              // There is a chunk to display info about. Process
              // strings.
              strnum(
                chunkPeekText[0], 11,
                debugChunk -> coordHash
              );
              strnum(
                chunkPeekText[1], 8,
                debugChunk -> loaded
              );
              // Draw the strings
              for(i = 0; i < 2; i++) {
                drawStr(renderer, chunkPeekText[i], 0, i << 3); 
              }
              
              // Scroll wheel for changing chunk map xray
              if(inputs->mouse_Wheel != 0) {
                chunkPeekRYMax -= inputs->mouse_Wheel;
                chunkPeekRYMax = nmod(chunkPeekRYMax, 64);
                inputs->mouse_Wheel = 0;
              }
              
              // Mouse for changing chunk map xray
              if(
                inputs->mouse_X > 128 &&
                inputs->mouse_Y < 64  &&
                inputs->mouse_Left
              ) chunkPeekRYMax = inputs->mouse_Y;
              
              // Up/Down buttons for changing chunk map xray
              if(button(renderer, "UP",
                4, 56, 64,
                inputs->mouse_X, inputs->mouse_Y)
                && inputs->mouse_Left
              ) {
                chunkPeekRYMax = nmod(chunkPeekRYMax - 1, 64);
              }
              
              if(button(renderer, "DOWN",
                4, 78, 64,
                inputs->mouse_X, inputs->mouse_Y)
                && inputs->mouse_Left
              ) {
                chunkPeekRYMax = nmod(chunkPeekRYMax + 1, 64);
              }
              
              // Draw chunk map
              white(renderer);
              SDL_RenderDrawLine(
                renderer,
                128, chunkPeekRYMax,
                191, chunkPeekRYMax
              );
              for(
                chunkPeekRY = 64;
                chunkPeekRY >= chunkPeekRYMax;
                chunkPeekRY--
              ) for(
                chunkPeekRX = 0;
                chunkPeekRX < 64;
                chunkPeekRX++
              ) for(
                chunkPeekRZ = 0;
                chunkPeekRZ < 64;
                chunkPeekRZ++
              ) {
                chunkPeekColor = textures[
                  debugChunk->blocks[
                    chunkPeekRX +
                    (chunkPeekRY << 6) +
                    (chunkPeekRZ << 12)
                  ] * 256 * 3 + 6 * 16
                ];
                if(chunkPeekColor) {
                  SDL_SetRenderDrawColor(
                    renderer,
                    (chunkPeekColor >> 16 & 0xFF),
                    (chunkPeekColor >> 8 & 0xFF),
                    (chunkPeekColor & 0xFF),
                    255
                  );
                  SDL_RenderDrawPoint(
                    renderer,
                    chunkPeekRX + 128,
                    chunkPeekRY + chunkPeekRZ
                  );
                  // A little shadow for depth
                  SDL_SetRenderDrawColor(
                    renderer,
                    0, 0, 0, 64
                  );
                  SDL_RenderDrawPoint(
                    renderer,
                    chunkPeekRX + 128,
                    chunkPeekRY + chunkPeekRZ + 1
                  );
                }
              }
            } else {
              drawStr(renderer, "NULL chunk!", 0, 0); 
            }
            
            if(button(renderer, "Done",
              4, 100, 64,
              inputs->mouse_X, inputs->mouse_Y) &&
              inputs->mouse_Left
            ) {
              gamePopup = 4;
            }
            break;
          
          // Chat
          case 6:
            // Chat history
            chatDrawIndex = chatHistoryIndex;
            for(i = 0; i < 11; i++) {
              chatDrawIndex = nmod(chatDrawIndex - 1, 11);
              drawBGStr(
                renderer, chatHistory[chatDrawIndex],
                0, BUFFER_H - 32 - i * 9
              );
            }
            
            // Get keyboard input
            if(inputs->keyTyped || inputs->keySym) {
              if(inputs->keySym == SDLK_BACKSPACE) {
                // Delete last char and decrement cursor
                // position
                if(chatBoxCursor > 0) {
                  chatBox[--chatBoxCursor] = 0;
                }
              } else if(
                inputs->keySym == SDLK_RETURN &&
                chatBoxCursor > 0
              ) {
                // Add input to chat
                chatAdd(
                  chatHistory,
                  chatHistoryFade,
                  &chatHistoryIndex,
                  chatBox
                );
                // Clear input box
                chatBoxCursor = 0;
                chatBox[0] = 0;
              } else if(
                inputs->keyTyped > 31 &&
                inputs->keyTyped < 127 &&
                chatBoxCursor < 64
              ) {
                chatBox[chatBoxCursor++] = inputs->keyTyped;
                chatBox[chatBoxCursor]   = 0;
              }
            }
            
            // Chat input box
            // If char limit is reached, give some visual
            // feedback.
            if(chatBoxCursor == 64)
              SDL_SetRenderDrawColor(renderer, 128, 0, 0, 128);
            else
              tblack(renderer);
            SDL_RenderFillRect(renderer, &chatBoxRect);
            
            white(renderer);
            drawChar(
              renderer,
              95 + 32 * ((gameTime >> 6) % 2),
              drawStr(
                renderer, chatBox,
                0, BUFFER_H - 8
              ),
              BUFFER_H - 8
            );
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
            
            // Text
            for(i = 0; i < 8; i++)
              drawBGStr(renderer, debugText[i], 0, i * 9);
          }
          // Hotbar
          tblack(renderer);
          SDL_RenderFillRect(renderer, &hotbarRect);
          
          hotbarSelectRect.x =
            BUFFER_HALF_W - 77 + hotbarSelect * 17;
          white(renderer);
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
          
          // Chat
          chatDrawIndex = chatHistoryIndex;
          for(i = 0; i < 11; i++) {
            chatDrawIndex = nmod(chatDrawIndex - 1, 11);
            if(chatHistoryFade[chatDrawIndex] > 0) {
              chatHistoryFade[chatDrawIndex]--;
              drawBGStr(
                renderer, chatHistory[chatDrawIndex],
                0, BUFFER_H - 32 - i * 9
              );
            }
          }
        }
      }
      
      if(inputs->mouse_Left) inputs->mouse_Left = 0;
      if(inputs->mouse_X)    inputs->mouse_X    = 0;
      
      // Clean up input struct
      inputs->keyTyped = 0;
      inputs->keySym   = 0;
      break;
    
    default:
      inputs->mouse_X /= BUFFER_SCALE;
      inputs->mouse_Y /= BUFFER_SCALE;
      
      dirtBg(renderer);
      white(renderer);
      centerStr(
        renderer,
        "Go away, this is my house.",
        BUFFER_HALF_W,
        BUFFER_HALF_H - 16
      );
      if(button(renderer, "Ok",
        BUFFER_HALF_W - 64, BUFFER_HALF_H, 128,
        inputs->mouse_X, inputs->mouse_Y) &&
        inputs->mouse_Left
      ) {
        gameState = 0;
      }
      break;
  }
  return 1;
}
