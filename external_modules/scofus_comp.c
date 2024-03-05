#include <stdio.h>
#include <stdlib.h>
#include "types.h"

ImageBatch run(ImageBatch *batch, int param) {
    ImageBatch data;
    data.mtype = 1;
    data.height = 1;
    data.width = 1;
    data.channels = 1;
    data.num_images = 1;
    size_t data_size = data.height * data.width * data.channels * data.num_images;
    data.data = (unsigned char *)malloc(data_size);
    unsigned char new_img[] = {batch->data[0]};
    memcpy(data.data, new_img, data_size);
    return data;
}