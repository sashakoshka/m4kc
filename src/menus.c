#include "menus.h"

void menu_hud (
  SDL_Renderer *renderer, Inputs *inputs,
  int *debugOn, u_int32_t *fps_now,
  Inventory *inventory, Coords *playerPosition
) {
  static SDL_Rect hotbarRect;
  static SDL_Rect hotbarSelectRect;
  
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

  int i;
    
  hotbarRect.x = BUFFER_HALF_W - 77;
  hotbarRect.y = BUFFER_H - 18;
  hotbarRect.w = 154;
  hotbarRect.h = 18;
  
  hotbarSelectRect.y = hotbarRect.y;
  hotbarSelectRect.w = 18;
  hotbarSelectRect.h = 18;

  // Debug screen
  if(*debugOn) {
    // Coordinates
    strnum(debugText[1], 3, (int)playerPosition->x - 64);
    strnum(debugText[2], 3, (int)playerPosition->y - 64);
    strnum(debugText[3], 3, (int)playerPosition->z - 64);
    
    // FPS
    strnum(debugText[4], 5, *fps_now);
    
    // Chunk coordinates
    strnum(
      debugText[5], 8, 
      ((int)playerPosition->x - 64) >> 6
    );
    strnum(
      debugText[6], 8, 
      ((int)playerPosition->y - 64) >> 6
    );
    strnum(
      debugText[7], 8, 
      ((int)playerPosition->z - 64) >> 6
    );
    
    // Text
    for(i = 0; i < 8; i++)
      drawBGStr(renderer, debugText[i], 0, i * 9);
  }
  // Hotbar
  tblack(renderer);
  SDL_RenderFillRect(renderer, &hotbarRect);
  
  hotbarSelectRect.x =
    BUFFER_HALF_W - 77 + inventory->hotbarSelect * 17;
  white(renderer);
  SDL_RenderDrawRect(renderer, &hotbarSelectRect);
  
  for(i = 0; i < 9; i++)
    drawSlot(
      renderer,
      &inventory->hotbar[i], 
      BUFFER_HALF_W - 76 + i * 17,
      BUFFER_H - 17,
      inputs->mouse_X,
      inputs->mouse_Y
    );
  
  // Chat
  int chatDrawIndex = chatHistoryIndex;
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

void menu_chat (SDL_Renderer *renderer, Inputs *inputs, long *gameTime) {
  static int  chatBoxCursor = 0;
  static char chatBox [64]  = {0};
  static SDL_Rect chatBoxRect = {0, 0, 0, 9};
  chatBoxRect.y = BUFFER_H - 9;
  chatBoxRect.w = BUFFER_W;

  // Chat history
  int chatDrawIndex = chatHistoryIndex;
  for(int i = 0; i < 11; i++) {
    chatDrawIndex = nmod(chatDrawIndex - 1, 11);
    drawBGStr(
      renderer, chatHistory[chatDrawIndex],
      0, BUFFER_H - 32 - i * 9
    );
  }
  
  // Get keyboard input
  if (inputs->keyTyped || inputs->keySym) {
    if (inputs->keySym == SDLK_BACKSPACE) {
      // Delete last char and decrement cursor
      // position
      if (chatBoxCursor > 0) {
        chatBox[--chatBoxCursor] = 0;
      }
    } else if (
      inputs->keySym == SDLK_RETURN &&
      chatBoxCursor > 0
    ) {
      // Add input to chat
      chatAdd(chatBox);
      // Clear input box
      chatBoxCursor = 0;
      chatBox[0] = 0;
    } else if (
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
  if (chatBoxCursor == 64)
    SDL_SetRenderDrawColor(renderer, 128, 0, 0, 128);
  else
    tblack(renderer);
  SDL_RenderFillRect(renderer, &chatBoxRect);
  
  white(renderer);
  drawChar(
    renderer,
    95 + 32 * ((*gameTime >> 6) % 2),
    drawStr(
      renderer, chatBox,
      0, BUFFER_H - 8
    ),
    BUFFER_H - 8
  );
}
