#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdint.h>
#include <string>

typedef struct ImageBatch {
    long mtype;          /* message type to read from the message queue */
    int height;          /* height of images */
    int width;           /* width of images */
    int channels;        /* channels of images */
    int num_images;      /* amount of images */
    int batch_size;      /* size of the image batch */
    int shm_key;         /* key to shared memory segment of image data */
    int pipeline_id;     /* id of pipeline to utilize for processing */
    u_char *data; /* address to image data (in shared memory) */
} ImageBatch;

typedef struct ImageBatchMessage {
    long mtype;
    u_int height;
    u_int width;
    u_int channels;
    u_int num_images;
    u_int batch_size;
    int shm_key;
} ImageBatchMessage;

typedef struct Image {
    size_t height;
    size_t width;
    size_t size;
    size_t bpp;
    u_char* data;
} Image;

enum struct CameraType {
    VMB,
    IR,
    Unkown,
};

typedef struct CaptureMessage {
    std::string CameraId;
    CameraType Type;
    size_t NumberOfImages;
    u_int Exposure;
    float ISO;
    u_int Interval;
} CaptureMessage;

// how many bytes in the beginning of the image buffer is allocated for metadata, which is just the size of the image
#define IMAGE_METADATA_SIZE 4

#define MSG_QUEUE_KEY 71

#endif