#include <stdlib.h>
#include <stdint.h>

typedef struct ImageBatch {
    long mtype;          /* message type to read from the message queue */
    int num_images;      /* amount of images */
    int batch_size;      /* size of the image batch */
    int shmid;           /* ID to shared memory segment of image data */
    int pipeline_id;     /* id of pipeline to utilize for processing */
    unsigned char *data; /* address to image data (in shared memory) */
} ImageBatch;