#include <stdio.h>
#include "data.h"
#include "string.h"
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

        int err = options_load();
        return err;
}

/* options_load
 * Loads options from the configuration file into the options struct. Returns 0
 * on success, non-zero on failure.
 */
int options_load (void) {
        const char *path = data_getOptionsFileName();

        // If the file doesn't exist, calmly exit the function and just use
        // default values
        FILE *file = fopen(path, "r");
        if (file == NULL) { return 0; }

        int version;
        fscanf(file, "%i", &version);
        if (version != 0) { return 1; }

        char parameter[16];
        while (1) {
                if (fscanf(file, "%15s", parameter) == EOF) { break; }

                #define PARAMETER(what, how, where) \
                        else if (strcmp(parameter, #what) == 0) { \
                                fscanf(file, how, where); \
                        }
                PARAMETER(fogType,      "%i",  &options.fogType)
                PARAMETER(drawDistance, "%i",  &options.drawDistance)
                PARAMETER(trapMouse,    "%i",  &options.trapMouse)
                PARAMETER(fov,          "%lf", &options.fov)
                PARAMETER(username,     "%7s", username)
                #undef PARAMETER
        }

        options.username.cursor = strlen(username);
        if (options.fogType > 1 || options.fogType < 0) { options.fogType = 0; }

        fclose(file);
        return 0;
}
