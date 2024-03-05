#include "../src/pipeline/module_config.pb-c.h"

typedef struct ImageBatch {
    long mtype;
    int height;
    int width;
    int channels;
    int num_images;
    int shm_key;
    int pipeline_id;
    unsigned char *data; // batched image data (255 different values)
} ImageBatch;