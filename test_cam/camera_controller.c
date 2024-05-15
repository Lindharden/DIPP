#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include "camera_controller.h"
#include "metadata.pb-c.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Missing arguments: Expected <num_images> <image_name>");
        return -1;
    }

    char * image_name = argv[2];

    // Get timestamp (used for SHM key)
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) < 0)
    {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    // Prepare the data
    ImageBatch data;
    data.mtype = 1;
    data.num_images = atoi(argv[1]);
    data.pipeline_id = 1;

    // Hardcoded bayer image specs
    uint32_t image_height = 1944;
    uint32_t image_width = 2592; 
    uint32_t bits_per_pixel = 12;
    uint32_t bytes_per_pixel = 2;
    uint32_t image_channels = 1;

    uint32_t image_size = image_height * image_width * image_channels * bytes_per_pixel;
    Metadata new_meta = METADATA__INIT;
    new_meta.size = image_size;
    new_meta.width = image_width;
    new_meta.height = image_height;
    new_meta.channels = image_channels;
    new_meta.timestamp = 0; // example time (should be using unix timestamp)
    new_meta.bits_pixel = bits_per_pixel;
    new_meta.camera = "rgb";

    const char filename[30];
    sprintf(filename, "images/%s", image_name);
    FILE *fh = fopen(filename, "r");

    size_t meta_size = metadata__get_packed_size(&new_meta);
    uint8_t meta_buf[meta_size];
    metadata__pack(&new_meta, meta_buf);

    uint32_t batch_size = (image_size + sizeof(uint32_t) + meta_size) * data.num_images;

    int shmid = shmget(time.tv_nsec, batch_size, IPC_CREAT | 0666);
    data.shmid = shmid;
    char *shmaddr = shmat(shmid, NULL, 0);
    data.batch_size = batch_size;
    int offset = 0;
    for (size_t i = 0; i < data.num_images; i++)
    {
        // Insert metadata size before metadata
        memcpy(shmaddr + offset, &meta_size, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(shmaddr + offset, &meta_buf, meta_size);
        offset += meta_size;
        // insert image
        fseek(fh, 0, SEEK_SET);
        fread(shmaddr + offset, 1, image_size, fh);
        offset += image_size;
    }

    // create msg queue
    int msg_queue_id;
    if ((msg_queue_id = msgget(71, 0666 | IPC_CREAT)) == -1)
    {
        perror("msgget error");
    }

    // send msg to queue
    if (msgsnd(msg_queue_id, &data, sizeof(data), 0) == -1)
    {
        perror("msgsnd error");
    }

    printf("Image sent!\n");

    // detach from the shared memory segment
    shmdt(shmaddr);
}