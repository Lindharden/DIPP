#include <stdio.h>
#include <stdlib.h>
#include "types.h"

ImageBatch run(ImageBatch *batch, int param) {
    ImageBatch new_batch;
    new_batch.mtype = batch->mtype;
    new_batch.width = batch->width;
    new_batch.height = batch->height;
    new_batch.channels = batch->channels;
    new_batch.num_images = batch->num_images;
    new_batch.data = batch->data;

    return new_batch;
}