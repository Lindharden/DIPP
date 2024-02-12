#include <stdio.h>
#include <stdlib.h>
#include "../src/pipeline/types.h"

ImageBatch run(ImageBatch *batch, int param) {
    // Create a new ImageBatch object to store the black and white image
    ImageBatch bwBatch;
    bwBatch.mtype = batch->mtype;
    bwBatch.height = batch->height;
    bwBatch.width = batch->width;
    bwBatch.channels = 1; // Black and white image will have only one channel
    bwBatch.num_images = batch->num_images;
    bwBatch.data = batch->data;

    int batch_size = batch->width * batch->height * batch->channels * batch->num_images;

    // Calculate the size of the black and white image data
    size_t bwDataSize = bwBatch.height * bwBatch.width * bwBatch.channels * bwBatch.num_images;
    unsigned char *new_data = (unsigned char *)malloc(bwDataSize);

    for (int i = 0; i < batch_size; i += batch->channels) {
        // Extract color components from the original pixel
        unsigned char r = batch->data[i];
        unsigned char g = batch->data[i + 1];
        unsigned char b = batch->data[i + 2];

        // Convert to grayscale using luminance method (0.299R + 0.587G + 0.114B)
        unsigned char gray = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);

        new_data[i / batch->channels] = gray;
    }

    memcpy(batch->data, new_data, bwDataSize);

    free(new_data);

    return bwBatch;
}