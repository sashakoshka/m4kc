#include "inputbuffer.h"

/* manageInputBuffer
 * Applies keyboard input to a text input buffer. Returns 1 if the enter key was
 * pressed (there must be content in buffer)
 */
int manageInputBuffer (InputBuffer *inputBuffer, Inputs *inputs) {
        if (!inputs->keyTyped && !inputs->keySym) { return 0; }
        
        if (inputs->keySym == SDLK_BACKSPACE) {
                // Delete last char and decrement cursor
                // position
                if (inputBuffer->cursor > 0) {
                        inputBuffer->buffer[-- inputBuffer->cursor] = 0;
                }
        } else if (inputs->keySym == SDLK_RETURN && inputBuffer->cursor > 0) {
                return 1;
        } else if (
                inputs->keyTyped > 31  &&
                inputs->keyTyped < 127 &&
                inputBuffer->cursor < inputBuffer->len - 1
        ) {
                inputBuffer->buffer[inputBuffer->cursor] = inputs->keyTyped;
                inputBuffer->cursor += 1;
                inputBuffer->buffer[inputBuffer->cursor] = 0;
        }

        return 0;
}
