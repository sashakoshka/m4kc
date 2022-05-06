#pragma once
#include <limits.h>
#include "inputbuffer.h"
#include "compat.h"

typedef struct {
        int buffer[16 * 16];
} data_WorldThumbnail;

typedef struct data_WorldListItem {
        data_WorldThumbnail thumbnail;
        char name[NAME_MAX];
        struct data_WorldListItem *next;
} data_WorldListItem;

extern data_WorldListItem *data_worldList;

typedef struct {
        int    fogType;
        int    drawDistance;
        int    trapMouse;
        double fov;
        InputBuffer username;
} data_Options;

extern data_Options data_options;

int data_init                  (void);
int data_directoryExists       (const char *);
int data_fileExists            (const char *);
int data_ensureDirectoryExists (const char *);

int  data_findDirectoryName   (char *, const char *);
char *data_getOptionsFileName (void);
int  data_getWorldPath        (char *, const char *);
void data_getWorldMetaPath    (char *, const char *);
void data_getWorldPlayerPath  (char *, const char *, const char *);
int  data_getScreenshotPath   (char *);

int data_refreshWorldList (void);
