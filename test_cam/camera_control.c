#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "camera_control.h"
#include "metadata.pb-c.h"

#define ONE_MB (1024 * 1024)

int main(int argc, char *argv[])
{
    int mb_size = 1;
    int num_iterations = 1;
    int num_modules = 1;
    if (argc < 4)
    {
        return -1;
    }
    mb_size = atoi(argv[1]);
    num_iterations = atoi(argv[2]);
    num_modules = atoi(argv[3]);
    
    // Prepare the data
    ImageBatch data;
    data.mtype = 1;
    data.num_images = mb_size;
    data.pipeline_id = num_modules;

    unsigned char one_mb_image_data[ONE_MB];
    unsigned char data_pattern = 0xAB;
    for (int i = 0; i < ONE_MB; i++) {
        one_mb_image_data[i] = data_pattern;
    }

    int fake_width = 1024;
    int fake_height = 1024;
    int fake_channels = 1;

    uint32_t image_size = fake_height * fake_width * fake_channels;
    
    Metadata new_meta = METADATA__INIT;
    new_meta.size = image_size;
    new_meta.width = fake_width;
    new_meta.height = fake_height;
    new_meta.channels = fake_channels;
    new_meta.timestamp = 0; // example time (should be using unix timestamp)
    new_meta.bits_pixel = 8;
    new_meta.camera = "rgb";

    size_t meta_size = metadata__get_packed_size(&new_meta);
    uint8_t meta_buf[meta_size];
    metadata__pack(&new_meta, meta_buf);

    uint32_t batch_size = (image_size + sizeof(uint32_t) + meta_size) * data.num_images;

    for (size_t i = 0; i < num_iterations; i++)
    {
        data.batch_size = batch_size;
        data.shm_key++;
        int shmid = shmget(data.shm_key, batch_size, IPC_CREAT | 0666);
        char *shmaddr = shmat(shmid, NULL, 0);
        int offset = 0;
        for (size_t i = 0; i < data.num_images; i++)
        {
            // Insert image size before image data
            memcpy(shmaddr + offset, &meta_size, sizeof(uint32_t));
            offset += sizeof(uint32_t);
            memcpy(shmaddr + offset, &meta_buf, meta_size);
            offset += meta_size;
            memcpy(shmaddr + offset, one_mb_image_data, image_size);
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
    }    
}
