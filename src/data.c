#include "data.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

Options data_options     = { 0 };
WorldListItem *worldList = NULL;

char directoryName            [PATH_MAX] = { 0 };
char settingsFileName         [PATH_MAX] = { 0 };
char worldsDirectoryName      [PATH_MAX] = { 0 };
char screenshotsDirectoryName [PATH_MAX] = { 0 };

char username[8] = "guest";

/* data_init
 * Initializes the data module. Returns zero on success, nonzero on failure.
 */
int data_init () {
        int err = 0;

        err = data_findDirectoryName(directoryName, "/.m4kc");
        if (err) { return err; }
        
        err = data_findDirectoryName(settingsFileName, "/.m4kc/m4kc.conf");
        if (err) { return err; }
        
        err = data_findDirectoryName(worldsDirectoryName, "/.m4kc/worlds");
        if (err) { return err; }
        
        err = data_findDirectoryName (
                screenshotsDirectoryName, "/.m4kc/screenshots");
        if (err) { return err; }

        data_options = (const Options) {
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

/* data_directoryExists
 * Test if a directory exists at the specified path. This function does not
 * create or delete anything.
 */
int data_directoryExists (const char *path) {
        struct stat directoryInfo;
        return (stat(path, &directoryInfo) == 0 &&
                S_ISDIR(directoryInfo.st_mode));
        
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

                if (!data_directoryExists(currentDirectory)) {
                        if (mkdir(currentDirectory, 0755) != 0) {
                                return 1;
                        }
                }
        }

        return 0;
}

/* data_findDirectoryName
 * Concatenates the user's home directory with the specified path. subDirectory
 * must begin with a '/'.
 */
int data_findDirectoryName (char *path, const char *subDirectory) {
        if (subDirectory[0] != '/') { return 2; }
        
        // TODO: make this work cross platform
        char *homeDirectory = getenv("HOME");
        if (homeDirectory == NULL) { return 3; }

        snprintf(path, PATH_MAX, "%s%s", homeDirectory, subDirectory);
        return 0;
}

/* data_getScreenshotPath
 * Writes into path the path for a new screenshot. The name will take the form
 * of: snip_YYYY-MM-DD_HH:MM:SS.bmp
 * ... and will be located in the path stored in screenshotsDirectoryName. If
 * the screenshots directory doesn't exist, this function will create it.
 */
int data_getScreenshotPath (char *path) {
        if (data_ensureDirectoryExists(screenshotsDirectoryName)) { return 1; }

        time_t unixTime = time(0);
        struct tm *timeInfo = localtime(&unixTime);
        
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
        return 0;
}

/* data_getWorldPath
 * Returns the path to a world, regardless if it exists or not. This function
 * ensures that the worlds directory exists. If it cannot do this, it returns 1.
 */
int data_getWorldPath (char *path, const char *worldName) {
        if (data_ensureDirectoryExists(worldsDirectoryName)) { return 1; }

        snprintf(path, PATH_MAX, "%s/%s", worldsDirectoryName, worldName);
        return 0;
}

/* data_refreshWorldList
 * Regreshes the world list, clearing the previous one. Reads world names and
 * thumbnails from ~/.m4kc/worlds
 */
int data_refreshWorldList () {
        // Free previous list
        WorldListItem *item = worldList;
        while (item != NULL) {
                WorldListItem *next = item->next;
                free(item);
                item = next;
        }

        if (data_ensureDirectoryExists(worldsDirectoryName)) { return 1; }
        
        // Iterate through worlds directory
        struct dirent *directoryEntry;
        DIR *directory = opendir(worldsDirectoryName);
        if (!directory) { return 2; }

        WorldListItem *last = NULL;
        while ((directoryEntry = readdir(directory)) != NULL) {
                if (directoryEntry->d_name[0] == '.') { continue; }

                // Allocate new list item
                WorldListItem *item = calloc(sizeof(WorldListItem), 1);
                if (item == NULL) { return 3; }
                
                strncpy(item->name, directoryEntry->d_name, NAME_MAX);
                if (last == NULL) {
                        worldList = item;
                        last = worldList;
                } else {
                        last->next = item;
                }
        }
        
        closedir(directory);

        return 0;
}
