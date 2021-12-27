#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "terrain.h"
#include "structs.h"
#include "textures.h"
#include "gui.h"
#include "gameloop.h"

/*
  Minecraft 4k, C edition. Version 0.7
  
  Credits:
    notch       - creating the original game
    sashakoshka - C port, modifications
    samsebe     - deciphering the meaning of some of the code
    gracie bell - daylight function
    https://gist.github.com/nowl/828013 - perlin noise
    samantaz    - de-obfuscating controls code
  
  If you distribute a modified copy of this, just include this
  notice.
*/

int main(/*int argc, char *argv[]*/) {
  Inputs inputs = {0};
  
  World world;
  
  //unsigned int seed = 18295169;
  unsigned int seed = 45390874;
  
  int mouseX = 0, mouseY = 0;
  
  //----  initializing SDL  ----//
  
  SDL_Window   *window   = NULL;
  SDL_Renderer *renderer = NULL;
  const Uint8  *keyboard = SDL_GetKeyboardState(NULL);
  SDL_Event     event;
  
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("cant make window\n");
    goto exit;
  }
  
  window = SDL_CreateWindow("M4KC",
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
  SDL_RenderSetScale(renderer, BUFFER_SCALE, BUFFER_SCALE);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  
  //---- generating assets  ----//
  
  genTextures(seed);
  
  initChunks(&world);
  
  //----   main game loop   ----//
  
  while(gameLoop(
    seed,
    &inputs,
    &world,
    renderer
  )) {
    
    SDL_PumpEvents();
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // Detect movement controls
    inputs.keyboard_Space = keyboard[SDL_SCANCODE_SPACE];
    inputs.keyboard_W     = keyboard[SDL_SCANCODE_W];
    inputs.keyboard_S     = keyboard[SDL_SCANCODE_S];
    inputs.keyboard_A     = keyboard[SDL_SCANCODE_A];
    inputs.keyboard_D     = keyboard[SDL_SCANCODE_D];
    
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
            inputs.keyboard_E   = keyboard[SDL_SCANCODE_E];
            inputs.keyboard_T   = keyboard[SDL_SCANCODE_T];
            
            inputs.keySym       = event.key.keysym.sym;
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
        
        case SDL_TEXTINPUT:
          inputs.keyTyped = event.text.text[0];
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
