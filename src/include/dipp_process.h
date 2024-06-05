#ifndef DIPP_PROCESS_H
#define DIPP_PROCESS_H

#include "dipp_config.h"

#define MSG_QUEUE_KEY 71

// Return codes
#define SUCCESS 0
#define FAILURE -1

// Pipeline run codes
typedef enum PIPELINE_PROCESS
{
    PROCESS_STOP = 0,
    PROCESS_ONE = 1,
    PROCESS_ALL = 2,
    PROCESS_WAIT_ONE = 3,
    PROCESS_WAIT_ALL = 4
} PIPELINE_PROCESS;

typedef struct ImageBatch {
    long mtype;          /* message type to read from the message queue */
    int num_images;      /* amount of images */
    int batch_size;      /* size of the image batch */
    int shmid;           /* key to shared memory segment of image data */
    int pipeline_id;     /* id of pipeline to utilize for processing */
    unsigned char *data; /* address to image data (in shared memory) */
} ImageBatch;

typedef ImageBatch (*ProcessFunction)(ImageBatch *, ModuleParameterList *, int *);

#endif