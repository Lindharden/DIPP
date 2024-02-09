#ifndef TYPES.H
#define TYPES.H

#include <stdlib.h>

typedef struct Data {
    long mtype;
    int value;
} Data;

typedef int (*ProcessFunction)(Data *, uint8_t);

typedef struct {
    ProcessFunction* functions;
    size_t size;
} Pipeline;

#endif // TYPES.H