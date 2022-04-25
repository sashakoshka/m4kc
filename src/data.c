#include "data.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *directoryName = NULL;
char *settingsFileName = NULL;
char *worldsDirectoryName = NULL;
char *screenshotsDirectoryName = NULL;

/* data_init
 * Initializes the data module
 */
int data_init () {
        directoryName = data_findDirectoryName("/.m4kc");
        if (directoryName == NULL) { return 1; }
        
        settingsFileName = data_findDirectoryName("/.m4kc/m4kc.conf");
        if (settingsFileName == NULL) { return 1; }
        
        worldsDirectoryName = data_findDirectoryName("/.m4kc/worlds");
        if (worldsDirectoryName == NULL) { return 1; }
        
        screenshotsDirectoryName = data_findDirectoryName("/.m4kc/screenshots");
        if (screenshotsDirectoryName == NULL) { return 1; }

        return 0;
}

/* data_findDirectoryName
 * concatenates the user's home directory with the specified path. Path must
 * begin with a slash.
 */
char *data_findDirectoryName(const char *path) {
        // TODO: make this work cross platform
        if (path[0] != '/') { return NULL; }
        
        char *homeDirectory = getenv("HOME");
        if (homeDirectory == NULL) { return NULL; }
        
        size_t offset = strlen(homeDirectory);
        size_t length = offset + sizeof(path);

        char *directory = calloc(length, sizeof(char));
        sprintf(directory, "%s%s", homeDirectory, path);
        
        return directory;
}
