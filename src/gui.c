#include "gui.h"

const int BUFFER_W     = 214;
const int BUFFER_H     = 120;
const int BUFFER_SCALE = 4;
const int BUFFER_HALF_W = BUFFER_W / 2;
const int BUFFER_HALF_H = BUFFER_H / 2;

char chatHistory      [11][64] = {0};
int  chatHistoryFade  [11]     = {0};
int  chatHistoryIndex          = 0;

/* strnum
 * Takes in a char array and an offset and puts the specified
 * number into it. Make sure there is sufficient space in the
 * string.
 */
void strnum (char *ptr, int offset, int num) {
        sprintf(ptr + offset, "%d", num);
}

/* drawChar
 * Takes in a pointer to a renderer, a character (as an int),
 * draws it at the specified x and y coordinates, and then returns
 * the character's width.
 */
int drawChar (SDL_Renderer *renderer,int c, int x, int y) {
        for (int yy = 0; yy < 8; yy++) {
                for (int xx = 0; xx < 8; xx++) {
                        if ((font[c][yy] >> (7 - xx)) & 0x1) {
                                SDL_RenderDrawPoint(renderer, x + xx, y + yy);
                        }
                }
        }

        return font[c][8];
}

/* drawStr
 * Takes in a pointer to a renderer, a string, draws it at the
 * specified x and y coordinates, and then returns the x position
 * it left off on.
 */
int drawStr (SDL_Renderer *renderer,char *str, int x, int y) {
        while (*str > 0) {
                x += drawChar(renderer, *(str++), x, y);
        }
        return x;
}

/* shadowStr
 * Identical to drawStr, but draws white text with a grey shadow.
 */
int shadowStr (SDL_Renderer *renderer, char *str, int x, int y) {
        SDL_SetRenderDrawColor(renderer, 77, 77, 77, 255);
        drawStr(renderer, str, x + 1, y + 1);
        white(renderer);
        return drawStr(renderer, str, x, y);
}

/* centerStr
 * Identical to drawStr, but centers the text
 */
int centerStr (SDL_Renderer *renderer, char *str, int x, int y) {
        x *= 2;
        char *strsave = str;
        while (*str > 0) {
                x -= font[(int)*(str++)][8];
        }

        str = strsave;
        x /= 2;

        while(*str > 0) {
                x += drawChar(renderer, *(str++), x, y);
        }

        return x;
}

/* shadowStr
 * Identical to centerStr, but draws white text with a grey shadow.
 */
int shadowCenterStr (SDL_Renderer *renderer, char *str, int x, int y) {
        SDL_SetRenderDrawColor(renderer, 77, 77, 77, 255);
        centerStr(renderer, str, x + 1, y + 1);
        white(renderer);
        return centerStr(renderer, str, x, y);
}

/* drawBig
 * Draws centered text at a large scale
 */
int drawBig (SDL_Renderer *renderer, char *str, int x, int y) {
        char *strsave = str;
        while (*str > 0)
        x -= font[(int)*(str++)][8];

        str = strsave;

        while (*str > 0) {
                int c = *(str++);
                for (int yy = 0; yy < 16; yy++) {
                        for (int xx = 0; xx < 16; xx++) {
                                if ((font[c][yy / 2] >> (7 - xx / 2)) & 0x1) {
                                        SDL_RenderDrawPoint (
                                                renderer,
                                                x + xx,
                                                y + yy
                                        );
                                }
                        }
                }

                x += font[c][8] * 2;
        }

        return x;
}

/* drawBGStr
 * Like drawStr, but also draws a semitransparent background
 * behind the text.
 */
int drawBGStr (
        SDL_Renderer *renderer,
        char *str, int x, int y
) {
        static int len;
        static SDL_Rect bg = {0, 0, 0, 9};
        static char *strBak;

        strBak = str;
        len = 0;
        while(*str > 0) {
                len += font[(int)*(str++)][8];
        }
        
        bg.x = x;
        bg.y = y;
        bg.w = len + 1;

        tblack(renderer);
        SDL_RenderFillRect(renderer, &bg);

        white(renderer);
        return drawStr(renderer, strBak, ++x, ++y);
}

/* button
 * Takes in a pointer to a renderer, a string, draws a button with
 * the specified x and y coordinates and width, and then returns
 * wether or not the specified mouse coordinates are within it.
 */
int button (
        SDL_Renderer *renderer,
        char *str,
        int x, int y, int w,
        int mouseX, int mouseY
) {
        int hover = mouseX >= x      &&
                    mouseY >= y      &&
                    mouseX <  x + w  &&
                    mouseY <  y + 16 ;

        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = 16;

        if (hover) {
                SDL_SetRenderDrawColor(renderer, 116, 134, 230, 255);
        } else {
                SDL_SetRenderDrawColor(renderer, 139, 139, 139, 255);
        }
        SDL_RenderFillRect(renderer, &rect);

        x += w / 2;
        y += 5;

        if (hover) {
                SDL_SetRenderDrawColor(renderer, 63,  63,  40,  255);
        } else {
                SDL_SetRenderDrawColor(renderer, 56,  56,  56,  255);
        }
        centerStr(renderer, str, x, y);

        x--;
        y--;

        if(hover) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 160, 255);
        } else {
                white(renderer);
        }
        centerStr(renderer, str, x, y);

        if(hover) {
                white(renderer);
        } else {
                SDL_SetRenderDrawColor(renderer, 0,   0,   0,   255);
        }
        SDL_RenderDrawRect(renderer, &rect);

        return hover;
}

/* drawSlot
 * Takes in a pointer to a renderer, an InvSlot, draws the item
 * with the specified x and y coordinates and width, and then 
 * returns wether or not the specified mouse coordinates are
 * within it.
 */
int drawSlot (
        SDL_Renderer *renderer,
        InvSlot *slot, int x, int y, int mouseX, int mouseY
) {
        int hover,
            i,
            xx,
            yy,
            color;

        char count[4];

        hover = mouseX >= x      &&
                mouseY >= y      &&
                mouseX <  x + 16 &&
                mouseY <  y + 16 ;

        if (slot->amount == 0) { return hover; }

        i = slot->blockid * 256 * 3;
        for (yy = 0; yy < 16; yy++) {
                for (xx = 0; xx < 16; xx++) {
                        color = textures[i];
                        SDL_SetRenderDrawColor(
                                renderer,
                                (color >> 16 & 0xFF),
                                (color >> 8 & 0xFF),
                                (color & 0xFF),
                                255
                        );
                        if (color > 0) {
                                SDL_RenderDrawPoint(renderer, x + xx, y + yy);
                        }
                        i++;
                }
        }

        strnum(count, 0, slot->amount);
        shadowStr(renderer, count, x + (slot->amount >= 10 ? 4 : 10), y + 8);

  return hover;
}

/* dirtBg
 * Draws a dirt textured background
 */
void dirtBg (SDL_Renderer *renderer) {
        int color;
        for (int y = 0; y < BUFFER_H; y++) {
                for (int x = 0; x < BUFFER_W; x++) {
                        color = textures [
                                (x & 0xF) +
                                (y & 0xF) * 16 +
                                2 * 256 * 3
                        ];
                        
                        SDL_SetRenderDrawColor(
                                renderer,
                                (color >> 16 & 0xFF) >> 1,
                                (color >> 8 & 0xFF)  >> 1,
                                (color & 0xFF)       >> 1,
                                255
                        );
                        
                        SDL_RenderDrawPoint(renderer, x, y);
                }
        }
}

/* loadScreen
 * Draws a loading screen
 */
void loadScreen (
        SDL_Renderer *renderer,
        char *str,
        float prog, float max
) {
        dirtBg(renderer);

        shadowCenterStr(renderer, str, BUFFER_HALF_W, BUFFER_HALF_H - 8);

        SDL_SetRenderDrawColor(renderer, 77, 77, 77, 255);
        SDL_RenderDrawLine (
                renderer,
                BUFFER_HALF_W - 32,
                BUFFER_HALF_H + 6,
                BUFFER_HALF_W + 32,
                BUFFER_HALF_H + 6
        );

        SDL_SetRenderDrawColor(renderer, 132, 255, 132, 255);
        SDL_RenderDrawLine ( 
                renderer,
                BUFFER_HALF_W - 32,
                BUFFER_HALF_H + 6,
                BUFFER_HALF_W - 32 + (prog / max) * 64,
                BUFFER_HALF_H + 6
        );
}

/* chatAdd
 * Adds a message to chat
 */
void chatAdd (char *str) {
        chatHistoryFade[chatHistoryIndex] = 480;
        memcpy (
                chatHistory[(chatHistoryIndex)++],
                str, sizeof(char) * 64
        );
        chatHistoryIndex = nmod(chatHistoryIndex, 11);
}

/* white
 * Sets the render color to white
 */
void white (SDL_Renderer *renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

/* tblack
 * Sets the render color to transparent black
 */
void tblack (SDL_Renderer *renderer) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
}
