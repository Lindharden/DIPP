#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "camera_control.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
    char input[100];

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
        const char *filename = "sim_image2.png"; // Change this to your image file
        int image_width, image_height, image_channels;
        unsigned char *image_data = stbi_load(filename, &image_width, &image_height, &image_channels, STBI_rgb_alpha);
        data.height = image_height;
        data.width = image_width;
        data.channels = image_channels;
        data.num_images = 1;
        data.shm_key = 1234; // testing key
        size_t data_size = image_height * image_width * image_channels * 1;
        int shmid = shmget(data.shm_key, 1024 * 1024 * 10, IPC_CREAT | 0666);
        char *shmaddr = shmat(shmid, NULL, 0);
        memcpy(shmaddr, image_data, data_size); // Copy image batch data to shared memory

        // create msg queue
        int msg_queue_id;
        if ((msg_queue_id = msgget(68, 0666 | IPC_CREAT)) == -1)
        {
            perror("msgget error");
        }

        // send msg to queue
        if (msgsnd(msg_queue_id, &data, sizeof(data) - sizeof(long), 0) == -1)
        {
            perror("msgsnd error");
        }

        printf("Image sent!\n");
    }
}
