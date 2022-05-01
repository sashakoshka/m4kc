#pragma once
#include "gui.h"

typedef struct {
        int   fogType;
        int   drawDistance;
        int   trapMouse;
        float fov;
        InputBuffer username;
} Options;

extern Options data_options;

int data_init ();
int data_directoryExists       (const char *);
int data_ensureDirectoryExists (const char *);

int data_findDirectoryName (char *, const char *);
int data_getScreenshotPath (char *);
int data_getWorldPath      (char *, const char *);
