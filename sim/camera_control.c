#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "camera_control.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char *argv[])
{
    int pipeline_id = 1;
    if (argc > 1)
    {
        pipeline_id = atoi(argv[1]);
    }
    char input[100];
    int i = 200;
    while (1)
    {
        printf("Type whatever to send image. Type 'quit' to exit.\n");
        scanf("%s", input);

        if (strcmp(input, "quit") == 0)
        {
            printf("Exiting camera sim...\n");
            break;
        }

        // Prepare the data
        ImageBatch data;
        data.mtype = 1;
        const char *filename = "sim_image.png"; 
        int image_width, image_height, image_channels;
        unsigned char *image_data = stbi_load(filename, &image_width, &image_height, &image_channels, STBI_rgb_alpha);
        data.height = image_height;
        data.width = image_width;
        data.channels = image_channels;
        data.num_images = atoi(input);
        data.shm_key = i += 20; // testing key
        data.pipeline_id = pipeline_id;
        size_t image_size = image_height * image_width * image_channels;
        size_t batch_size = (image_size + sizeof(uint32_t)) * data.num_images;
        int shmid = shmget(data.shm_key, batch_size, IPC_CREAT | 0666);
        char *shmaddr = shmat(shmid, NULL, 0);
        data.batch_size = batch_size;
        int offset = 0;
        for (size_t i = 0; i < data.num_images; i++)
        {
            // Insert image size before image data
            memcpy(shmaddr + offset, &image_size, sizeof(uint32_t));
            offset += sizeof(uint32_t);
            memcpy(shmaddr + offset, image_data, image_size);
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
    }
}
