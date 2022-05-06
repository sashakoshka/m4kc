#include "options.h"

Options options = { 0 };
char username[8] = "guest";

/* options_init
 * Initializes the options module. Returns zero on success, nonzero on failure.
 */
int options_init (void) {
        options = (const Options) {
                .fogType      = 0,
                .drawDistance = 20,
                .trapMouse    = 0,
                .fov          = 90.0,
                .username     = (const InputBuffer){
                        .buffer = username,
                        .len    = 8,
                        .cursor = 5
                }
        };

        return 0;
}
