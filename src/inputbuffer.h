#pragma once

#include <stdlib.h>
#include "main.h"

typedef struct {
        char   *buffer;
        size_t len;
        size_t cursor;
} InputBuffer;

int manageInputBuffer (InputBuffer *, Inputs *);
