#pragma once
#include "inputbuffer.h"

typedef struct {
        int    fogType;
        int    drawDistance;
        int    trapMouse;
        double fov;
        InputBuffer username;
} Options;

extern Options options;

int options_init (void);
