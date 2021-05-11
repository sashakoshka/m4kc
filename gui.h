void strnum  (char*, int, int);
int drawChar (SDL_Renderer*,   int, int, int);
int drawStr  (SDL_Renderer*, char*, int, int);
int button   (SDL_Renderer*, char*,
  int, int, int, int, int
);
int drawSlot (SDL_Renderer*, InvSlot*,
  int, int, int, int
);

/*
  strnum
  Takes in a char array and an offset and puts the specifiec
  number into it. Make sure there is sufficient space in the
  string.
*/
void strnum(char *ptr, int offset, int num) {
  sprintf(ptr + offset, "%d", num);
}

/*
  drawChar
  Takes in a pointer to a renderer, a charachter (as an int),
  draws it at the specified x and y coordinates, and then returns
  the charachter's width.
*/
int drawChar(SDL_Renderer *renderer,
  int c, int x, int y
) {
  for(int yy = 0; yy < 8; yy++) {
    for(int xx = 0; xx < 8; xx++) {
      if((font[c][yy] >> (7 - xx)) & 0x1)
        SDL_RenderDrawPoint(renderer, x + xx, y + yy);
    }
  }
  
  return font[c][8];
}

/*
  drawStr
  Takes in a pointer to a renderer, a string, draws it at the
  specified x and y coordinates, and then returns the x position
  it left off on.
*/
int drawStr(SDL_Renderer *renderer,
  char *str, int x, int y
) {
  while(*str > 0) {
    x += drawChar(renderer, *(str++), x, y);
  }
  
  return x;
}

/*
  button
  Takes in a pointer to a renderer, a string, draws a button with
  the specified x and y coordinates and width, and then returns
  wether or not the specified mouse coordinates are within it.
*/
int button(SDL_Renderer *renderer,
  char *str, int x, int y, int w, int mouseX, int mouseY
) {
  int hover =
    mouseX >= x      &&
    mouseY >= y      &&
    mouseX <  x + w  &&
    mouseY <  y + 16 ;
  
  char *strsave = str;
  int len = 0;
  while(*str > 0) {
    len += font[(int)*(str++)][8];
  }
  str = strsave;
  
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = 16;
  
  if(hover)
    SDL_SetRenderDrawColor(renderer, 116, 134, 230, 255);
  else
    SDL_SetRenderDrawColor(renderer, 139, 139, 139, 255);
  SDL_RenderFillRect(renderer, &rect);
  
  x += (w - len) / 2 + 1;
  y += 5;
  
  if(hover)
    SDL_SetRenderDrawColor(renderer, 63,  63,  40,  255);
  else
    SDL_SetRenderDrawColor(renderer, 56,  56,  56,  255);
  drawStr(renderer, str, x, y);
  
  x--;
  y--;
  
  if(hover)
    SDL_SetRenderDrawColor(renderer, 255, 255, 160, 255);
  else
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  drawStr(renderer, str, x, y);
  
  if(hover)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  else
    SDL_SetRenderDrawColor(renderer, 0,   0,   0,   255);
  SDL_RenderDrawRect(renderer, &rect);
  
  return hover;
}

/*
  drawSlot
  Takes in a pointer to a renderer, an InvSlot, draws the item
  with the specified x and y coordinates and width, and then 
  returns wether or not the specified mouse coordinates are
  within it.
*/
int drawSlot(SDL_Renderer *renderer,
  InvSlot *slot, int x, int y, int mouseX, int mouseY
) {
  static int hover,
             i,
             xx,
             yy,
             color;
  
  hover =
    mouseX >= x      &&
    mouseY >= y      &&
    mouseX <  x + 16 &&
    mouseY <  y + 16 ;
  
  i = slot->blockid * 256 * 3;
  for(yy = 0; yy < 16; yy++)
    for(xx = 0; xx < 16; xx++) {
      color = textures[i];
      SDL_SetRenderDrawColor(
        renderer,
        (color >> 16 & 0xFF),
        (color >> 8 & 0xFF),
        (color & 0xFF),
        255
      );
      if(color > 0)
        SDL_RenderDrawPoint(renderer, x + xx, y + yy);
      i++;
    }
  
  return hover;
}
