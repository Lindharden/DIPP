#include <stdio.h>
#include <stdlib.h>
#include "../src/pipeline/types.h"

ImageBatch run(ImageBatch *batch, int param) {    
    ImageBatch mirroredBatch;
    mirroredBatch.mtype = batch->mtype;
    mirroredBatch.height = batch->height;
    mirroredBatch.width = batch->width; // Width remains the same
    mirroredBatch.channels = batch->channels;
    mirroredBatch.num_images = batch->num_images;
    mirroredBatch.data_size = batch->data_size;
    mirroredBatch.data = batch->data;

    // Mirror the image horizontally
    for (int i = 0; i < batch->num_images; i++) {
        for (int j = 0; j < batch->height; j++) {
            // Calculate source and destination offsets for memcpy
            int src_offset = i * batch->height * batch->width * batch->channels + j * batch->width * batch->channels;
            int dest_offset = i * mirroredBatch.height * mirroredBatch.width * mirroredBatch.channels + j * mirroredBatch.width * mirroredBatch.channels;

            // Copy the row in reverse order
            for (int k = 0; k < batch->width; k++) {
                for (int c = 0; c < batch->channels; c++) {
                    mirroredBatch.data[dest_offset + (batch->width - k - 1) * batch->channels + c] = batch->data[src_offset + k * batch->channels + c];
                }
            }
        }
    }

    return mirroredBatch;
}