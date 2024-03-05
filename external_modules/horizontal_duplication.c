#include <stdio.h>
#include <stdlib.h>
#include "types.h"

ImageBatch run(ImageBatch *batch, int param) {
    ImageBatch enlargedBatch;
    enlargedBatch.mtype = batch->mtype;
    enlargedBatch.height = batch->height;
    enlargedBatch.width = batch->width * (1 + param); // Increase width by param times
    enlargedBatch.shm_key = batch->shm_key;
    enlargedBatch.channels = batch->channels;
    enlargedBatch.num_images = batch->num_images;

    size_t enlarged_data_size = enlargedBatch.height * enlargedBatch.width * enlargedBatch.channels * enlargedBatch.num_images;
    enlargedBatch.data = (unsigned char *)malloc(enlarged_data_size);

    // Duplicate the image horizontally
    for (int i = 0; i < batch->num_images; i++) {
        for (int j = 0; j < batch->height; j++) {
            // Copy the original row
            memcpy(&enlargedBatch.data[i * enlargedBatch.height * enlargedBatch.width * enlargedBatch.channels + j * enlargedBatch.width * enlargedBatch.channels],
                   &batch->data[i * batch->height * batch->width * batch->channels + j * batch->width * batch->channels],
                   batch->height * batch->width * batch->channels);
            // Duplicate the row param times
            for (int k = 1; k <= param; k++) {
                memcpy(&enlargedBatch.data[i * enlargedBatch.height * enlargedBatch.width * enlargedBatch.channels + (j * enlargedBatch.width + k * batch->width) * enlargedBatch.channels],
                    &batch->data[i * batch->height * batch->width * batch->channels + j * batch->width * batch->channels],
                    batch->width * batch->channels); // Copy a single row's worth of data
            }
        }
    }

    return enlargedBatch;
}