#include <stdlib.h>
#include <stdint.h>

typedef struct ImageBatch {
    long mtype;
    int height;
    int width;
    int channels;
    int num_images;
    int shm_key;
    int pipeline_id;
    unsigned char *data;
} ImageBatch;