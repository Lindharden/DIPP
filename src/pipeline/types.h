#ifndef TYPES.H
#define TYPES.H

#include <stdlib.h>
#include <stdint.h>
#include "module_config.pb-c.h"

typedef struct ImageBatch {
    long mtype;
    int height;
    int width;
    int channels;
    int num_images;
    int shm_key;
    unsigned char *data; // batched image data (255 different values)
} ImageBatch;

typedef ImageBatch (*ProcessFunction)(ImageBatch *, ModuleConfig *);

typedef struct {
    ProcessFunction* functions;
    size_t size;
} Pipeline;

#endif // TYPES.H