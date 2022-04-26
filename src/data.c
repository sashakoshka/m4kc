#include "data.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

char *directoryName = NULL;
char *settingsFileName = NULL;
char *worldsDirectoryName = NULL;
char *screenshotsDirectoryName = NULL;

/* data_init
 * Initializes the data module. Returns 1 on failure, 0 on success.
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
 * Concatenates the user's home directory with the specified path. Path must
 * begin with a slash.
 */
char *data_findDirectoryName (const char *path) {
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

/* data_ensureDirectoryExists
 * Equivalent to mkdir -p. Creates a directory and all of its parent directories
 * if they don't exist.
 */
int data_ensureDirectoryExists (const char *path) {
        char currentDirectory[PATH_MAX] = "/";
        int index = 1;
        
        while (path[index] != 0) {
                while (path[index] != 0) {
                        currentDirectory[index] = path[index];
                        currentDirectory[index + 1] = 0;
                        index ++;
                        if (currentDirectory[index - 1] == '/') {
                                break;
                        }
                }

                struct stat directoryInfo;
                if (
                        stat(currentDirectory, &directoryInfo) != 0 ||
                        !S_ISDIR(directoryInfo.st_mode)
                ) {
                        if (mkdir(currentDirectory, 0755) != 0) {
                                return 1;
                        }
                }
        }

        return 0;
}

/* data_getScreenshotPath
 * Returns a path for a new screenshot. The name will take the form of:
 * snip_YYYY-MM-DD_HH:MM:SS.bmp
 * ... and will be located in the path stored in screenshotsDirectoryName. If
 * the screenshots directory doesn't exist, this function will create it.
 */
char *data_getScreenshotPath () {
        if (data_ensureDirectoryExists(screenshotsDirectoryName)) {
                return NULL;
        }

        time_t unixTime = time(0);
        struct tm *timeInfo = localtime(&unixTime);

        char *path = calloc(PATH_MAX, sizeof(char));
        if (path == NULL) {
                return NULL;
        }
        
        snprintf (
                path, PATH_MAX,
                "%s/snip_%04i-%02i-%02i_%02i-%02i-%02i.bmp",
                screenshotsDirectoryName,
                timeInfo->tm_year + 1900,
                timeInfo->tm_mon  + 1,
                timeInfo->tm_mday,
                timeInfo->tm_hour,
                timeInfo->tm_min,
                timeInfo->tm_sec);
        return path;
}

