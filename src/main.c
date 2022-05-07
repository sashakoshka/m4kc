#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "textures.h"
#include "gameloop.h"
#include "options.h"
#include "terrain.h"
#include "data.h"
#include "main.h"
#include "gui.h"

/* Minecraft 4k, C edition. Version 0.7
 * 
 * Credits:
 *   notch       - creating the original game
 *   sashakoshka - C port, modifications
 *   samsebe     - deciphering the meaning of some of the code
 *   gracie bell - daylight function
 *   https://gist.github.com/nowl/828013 - perlin noise
 *   
 *   ... & contributors on github!
 *   https://github.com/sashakoshka/m4kc/graphs/contributors
 * 
 * If you distribute a modified copy of this, just include this
 * notice.
 */
 
#define MAX_FPS 60
#define MIN_FRAME_MILLISECONDS 1000 / MAX_FPS

static int controlLoop(Inputs *, const uint8_t *);
static int handleEvent(Inputs *, const uint8_t *, SDL_Event);

int main (int argc, char *argv[]) {
        (void)(argc);
        (void)(argv);

        //----  initializing SDL  ----//

        SDL_Window   *window   = NULL;
        SDL_Renderer *renderer = NULL;
        const uint8_t  *keyboard = SDL_GetKeyboardState(NULL);

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                printf("cant make window\n");
                goto exit;
        }

        window = SDL_CreateWindow ("M4KC",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                WINDOW_W, WINDOW_H,
                SDL_WINDOW_SHOWN
        );
        if (window == NULL) {
                printf("%s\n", SDL_GetError());
                goto exit;
        }

        renderer = SDL_CreateRenderer (
                window,
                -1, SDL_RENDERER_ACCELERATED
        );
        if (renderer == NULL) {
                printf("%s\n", SDL_GetError());
                goto exit;
        }
        SDL_RenderSetScale(renderer, BUFFER_SCALE, BUFFER_SCALE);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        //--- initializing modules ---//
        
        int err = 0;
        
        err = data_init();
        if (err) {
                gameLoop_error("Cannot initialize data module.");
        }
        
        err = options_init();
        if (err) {
                gameLoop_error("Cannot initialize options module.");
        }

        //---- generating assets  ----//

        genTextures(45390874);

        //----   main game loop   ----//

        Inputs inputs = {0};
        int running = 1;
        while (running) {
                uint32_t frameStartTime = SDL_GetTicks();
                
                running &= controlLoop(&inputs, keyboard);
                running &= gameLoop(&inputs, renderer);
                
                SDL_RenderPresent(renderer);
                SDL_UpdateWindowSurface(window);

                // Clean up input struct
                inputs.keyTyped = 0;
                inputs.keySym   = 0;

                // Limit FPS
                uint32_t frameDuration = SDL_GetTicks() - frameStartTime;
                if (frameDuration < MIN_FRAME_MILLISECONDS) {
                        SDL_Delay(MIN_FRAME_MILLISECONDS - frameDuration);
                }
        }

        exit:
        SDL_Quit();
        return 0;
}

static int controlLoop (Inputs *inputs, const Uint8 *keyboard) {
        SDL_PumpEvents();
        int mouseX = 0, mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Detect movement controls
        inputs->keyboard.space = keyboard[SDL_SCANCODE_SPACE];
        inputs->keyboard.w     = keyboard[SDL_SCANCODE_W];
        inputs->keyboard.s     = keyboard[SDL_SCANCODE_S];
        inputs->keyboard.a     = keyboard[SDL_SCANCODE_A];
        inputs->keyboard.d     = keyboard[SDL_SCANCODE_D];

        if (!SDL_GetRelativeMouseMode()) {
                inputs->mouse.x = mouseX;
                inputs->mouse.y = mouseY;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
                if (!handleEvent(inputs, keyboard, event)) {
                        return 0;
                }
        }

        return 1;
}

static int handleEvent (Inputs *inputs, const uint8_t *keyboard, SDL_Event event) {
        switch (event.type) {
        case SDL_QUIT:
                return 0;
          
        case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                        inputs->mouse.left = 1;
                        break;
                case SDL_BUTTON_RIGHT:
                        inputs->mouse.right = 1;
                        break;
                }
                break;
          
        case SDL_MOUSEBUTTONUP:
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                        inputs->mouse.left = 0;
                        break;
                case SDL_BUTTON_RIGHT:
                        inputs->mouse.right = 0;
                        break;
                }
                break;

        case SDL_KEYDOWN:
                inputs->keySym = event.key.keysym.sym;
        case SDL_KEYUP:
                if (event.key.repeat == 0) {
                        // Detect UI hotkeys
                        inputs->keyboard.esc = keyboard[SDL_SCANCODE_ESCAPE];
                        inputs->keyboard.f1  = keyboard[SDL_SCANCODE_F1];
                        inputs->keyboard.f2  = keyboard[SDL_SCANCODE_F2];
                        inputs->keyboard.f3  = keyboard[SDL_SCANCODE_F3];
                        inputs->keyboard.f4  = keyboard[SDL_SCANCODE_F4];
                        inputs->keyboard.e   = keyboard[SDL_SCANCODE_E];
                        inputs->keyboard.t   = keyboard[SDL_SCANCODE_T];
                        inputs->keyboard.f   = keyboard[SDL_SCANCODE_F];

                        inputs->keyboard.num0 = keyboard[SDL_SCANCODE_0];
                        inputs->keyboard.num1 = keyboard[SDL_SCANCODE_1];
                        inputs->keyboard.num2 = keyboard[SDL_SCANCODE_2];
                        inputs->keyboard.num3 = keyboard[SDL_SCANCODE_3];
                        inputs->keyboard.num4 = keyboard[SDL_SCANCODE_4];
                        inputs->keyboard.num5 = keyboard[SDL_SCANCODE_5];
                        inputs->keyboard.num6 = keyboard[SDL_SCANCODE_6];
                        inputs->keyboard.num7 = keyboard[SDL_SCANCODE_7];
                        inputs->keyboard.num8 = keyboard[SDL_SCANCODE_8];
                        inputs->keyboard.num9 = keyboard[SDL_SCANCODE_9];
                }
                
                break;

        case SDL_MOUSEWHEEL:
                inputs->mouse.wheel = event.wheel.y;
                break;

        case SDL_MOUSEMOTION:
                if (SDL_GetRelativeMouseMode()) {
                        inputs->mouse.x = event.motion.xrel;
                        inputs->mouse.y = event.motion.yrel;
                }
                break;

        case SDL_TEXTINPUT:
                inputs->keyTyped = event.text.text[0];
                break;
        }

        return 1;
}
