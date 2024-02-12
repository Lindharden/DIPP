#include <stdio.h>
#include <stdlib.h>
#include "../src/pipeline/types.h"

ImageBatch run(const ImageBatch *input_batch, int param) {
    ImageBatch mirrored_batch;
    mirrored_batch.mtype = input_batch->mtype;
    mirrored_batch.width = input_batch->width;
    mirrored_batch.height = input_batch->height;
    mirrored_batch.channels = input_batch->channels;
    mirrored_batch.shm_key = input_batch->shm_key;
    mirrored_batch.num_images = input_batch->num_images;
    mirrored_batch.data = input_batch->data;

    // Calculate the size of the entire batch
    int data_size = input_batch->width * input_batch->height * input_batch->channels * input_batch->num_images;   
    unsigned char *new_data = (unsigned char *)malloc(data_size);

    // Calculate the size of each image in bytes
    size_t image_size = input_batch->height * input_batch->width * input_batch->channels;

    // Mirror each image horizontally
    for (int i = 0; i < input_batch->num_images; ++i) {
        unsigned char *image_data = &mirrored_batch.data[i * image_size];
        for (int y = 0; y < input_batch->height; ++y) {
            for (int x = 0; x < input_batch->width / 2; ++x) {
                // Swap pixel values horizontally
                for (int c = 0; c < input_batch->channels; ++c) {
                    unsigned char temp = image_data[(y * input_batch->width + x) * input_batch->channels + c];
                    new_data[(y * input_batch->width + x) * input_batch->channels + c] =
                        image_data[(y * input_batch->width + (input_batch->width - 1 - x)) * input_batch->channels + c];
                    new_data[(y * input_batch->width + (input_batch->width - 1 - x)) * input_batch->channels + c] = temp;
                }
            }
        }
    }

    memcpy(input_batch->data, new_data, data_size);

    free(new_data);

    return mirrored_batch;
}