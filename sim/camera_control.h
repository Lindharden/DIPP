#include <stdlib.h>
#include <stdint.h>

typedef struct ImageBatch {
    long mtype;
    int height;
    int width;
    int channels;
    int num_images;
    int shm_key;
    int pipeline_id; // which pipeline to utilize
    unsigned char *data; // batched image data (255 different values)
} ImageBatch;