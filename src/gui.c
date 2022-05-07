#include "gui.h"
#include "blocks.h"

const int BUFFER_W     = 214;
const int BUFFER_H     = 120;
const int BUFFER_SCALE = 4;
const int BUFFER_HALF_W = BUFFER_W / 2;
const int BUFFER_HALF_H = BUFFER_H / 2;
const int WINDOW_W     = BUFFER_W * BUFFER_SCALE;
const int WINDOW_H     = BUFFER_H * BUFFER_SCALE;

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
int drawStr (SDL_Renderer *renderer, const char *str, int x, int y) {
        while (*str > 0) {
                x += drawChar(renderer, *(str++), x, y);
        }
        return x;
}

/* shadowStr
 * Identical to drawStr, but draws white text with a grey shadow.
 */
int shadowStr (SDL_Renderer *renderer, const char *str, int x, int y) {
        SDL_SetRenderDrawColor(renderer, 77, 77, 77, 255);
        drawStr(renderer, str, x + 1, y + 1);
        white(renderer);
        return drawStr(renderer, str, x, y);
}

/* centerStr
 * Identical to drawStr, but centers the text
 */
int centerStr (SDL_Renderer *renderer, const char *str, int x, int y) {
        x *= 2;
        int i = 0;
        while (str[i] > 0) {
                x -= font[(int)str[i++]][8];
        }

        x /= 2;
        i = 0;
        while(str[i] > 0) {
                x += drawChar(renderer, str[i++], x, y);
        }

        return x;
}

/* shadowStr
 * Identical to centerStr, but draws white text with a grey shadow.
 */
int shadowCenterStr (SDL_Renderer *renderer, const char *str, int x, int y) {
        SDL_SetRenderDrawColor(renderer, 77, 77, 77, 255);
        centerStr(renderer, str, x + 1, y + 1);
        white(renderer);
        return centerStr(renderer, str, x, y);
}

/* drawBig
 * Draws centered text at a large scale
 */
int drawBig (SDL_Renderer *renderer, const char *str, int x, int y) {
        int i = 0;
        while (str[i] > 0)
                x -= font[(int)str[i++]][8];

        i = 0;
        while (str[i] > 0) {
                int c = str[i++];
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
        const char *str, int x, int y
) {
        static int len;
        static SDL_Rect bg = {0, 0, 0, 9};

        int i = 0;
        len = 0;
        while(str[i] > 0) {
                len += font[(int)str[i++]][8];
        }
        
        bg.x = x;
        bg.y = y;
        bg.w = len + 1;

        tblack(renderer);
        SDL_RenderFillRect(renderer, &bg);

        white(renderer);
        return drawStr(renderer, str, ++x, ++y);
}

/* button
 * Takes in a pointer to a renderer, a string, draws a button with
 * the specified x and y coordinates and width, and then returns
 * wether or not the specified mouse coordinates are within it.
 */
int button (
        SDL_Renderer *renderer,
        const char *str,
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

        x += w / 2 + 1;
        y += 5;

        if (hover) {
                SDL_SetRenderDrawColor(renderer, 63,  63,  40,  255);
        } else {
                SDL_SetRenderDrawColor(renderer, 56,  56,  56,  255);
        }
        centerStr(renderer, str, x, y);

        x--;
        y--;

        if (hover) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 160, 255);
        } else {
                white(renderer);
        }
        centerStr(renderer, str, x, y);

        if (hover) {
                white(renderer);
        } else {
                SDL_SetRenderDrawColor(renderer, 0,   0,   0,   255);
        }
        SDL_RenderDrawRect(renderer, &rect);

        return hover;
}

/* input
 * Draws a simple text input. Draws the string in buffer. If buffer is empty,
 * the placeholder text is drawn instead (but in a darker color). Returns
 * whether or not the mouse is inside of the input.
 */
int input (
        SDL_Renderer *renderer,
        const char *placeholder,
        const char *buffer,
        int x, int y, int w,
        int mouseX, int mouseY,
        int active
) {
        static int flash = 0;

        int hover = mouseX >= x      &&
                    mouseY >= y      &&
                    mouseX <  x + w  &&
                    mouseY <  y + 16 ;

        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = 16;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        
        if (hover || active) {
                white(renderer);
        } else {
                SDL_SetRenderDrawColor(renderer, 139, 139, 139, 255);
        }
        SDL_RenderDrawRect(renderer, &rect);

        int textX = x + 4;
        if (buffer[0] != 0) {
                white(renderer);
                textX = drawStr(renderer, buffer, x + 4, y + 4);
        } else {
                SDL_SetRenderDrawColor(renderer, 63, 63, 63, 255);
                drawStr(renderer, placeholder, x + 4, y + 4);
        }

        if (flash < 32 && active) {
                white(renderer);
                drawChar(renderer, '_', textX, y + 4);
        }

        flash ++;
        flash %= 64;

        return hover;
}

/* scrollbar
 * Draws a scrollbar of the specified length at the specified coordinates.
 * Able to modify the level value when the user interacts with it.
 */
void scrollbar (
        SDL_Renderer *renderer,
        int x, int y, int length,
        int mouseX, int mouseY,
        int mouseLeft,
        int *level, int max
) {
        float sectionLength = (float)length / (float)max;

        SDL_Rect background = {
                .x = x,
                .y = y,
                .w = 4,
                .h = length
        };

        SDL_Rect foreground = {
                .x = x,
                .y = (float)*level * sectionLength,
                .w = 4,
                .h = ceil(sectionLength)
        };

        int hover = mouseX >= background.x                &&
                    mouseY >= background.y                &&
                    mouseX <  background.x + background.w &&
                    mouseY <  background.y + background.h;

        if (hover && mouseLeft) {
                *level = (mouseY - background.y) / sectionLength;
        }

        tblack(renderer);
        SDL_RenderFillRect(renderer, &background);
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &foreground);
        SDL_SetRenderDrawColor(renderer, 139, 139, 139, 255);
        SDL_RenderDrawLine (
                renderer,
                foreground.x + foreground.w - 1,
                foreground.y,
                foreground.x + foreground.w - 1,
                foreground.y + foreground.h - 1);
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
                        color = textures[i + BLOCK_TEXTURE_H * BLOCK_TEXTURE_W];
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

/* drawWorldListItem
 * Draws a world list item and reurns whether the mouse is inside of it or one
 * of its buttons.
 */
int drawWorldListItem (
        SDL_Renderer *renderer, data_WorldListItem *item,
        int x, int y, int mouseX, int mouseY
) {
        int hover;
        
        hover = mouseX >= x       &&
                mouseY >= y       &&
                mouseX <  x + 128 &&
                mouseY <  y + 16 ;

        SDL_Rect rect;
        rect.x = x - 1;
        rect.y = y - 1;
        rect.w = 130;
        rect.h = 18;

        SDL_Rect thumbnailShadow;
        thumbnailShadow.x = x + 1;
        thumbnailShadow.y = y + 1;
        thumbnailShadow.w = 16;
        thumbnailShadow.h = 16;

        tblack(renderer);
        SDL_RenderFillRect(renderer, &thumbnailShadow);
        
        int *pixel = item->thumbnail.buffer;
        for (int yy = 0; yy < 16; yy++) {
                for (int xx = 0; xx < 16; xx++) {
                        SDL_SetRenderDrawColor (
                                renderer,
                                (*pixel >> 16 & 0xFF),
                                (*pixel >> 8 & 0xFF),
                                (*pixel & 0xFF),
                                255
                        );
                        SDL_RenderDrawPoint(renderer, x + xx, y + yy);
                        pixel ++;
                }
        }

        shadowStr(renderer, item->name, x + 20, y + 4);

        if (button(renderer, "x",
                x + 128 - 16, y, 16,
                mouseX, mouseY)
        ) {
                hover = 2;
        }

        if (hover == 1) {
                white(renderer);
                SDL_RenderDrawRect(renderer, &rect);
        }

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
                                BLOCK_DIRT * 256 * 3
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
        const char *str,
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
void chatAdd (const char *str) {
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
