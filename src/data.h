#pragma once

int  data_init ();
int  data_directoryExists       (const char *);
int  data_ensureDirectoryExists (const char *);
char *data_findDirectoryName    (const char *);
char *data_getScreenshotPath    ();
char *data_getWorldPath         (const char *);
