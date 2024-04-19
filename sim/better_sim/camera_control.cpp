#include "message_queue.hpp"
#include "common.hpp" // assuming this contains definitions for u_int16_t and u_char
#include <iostream>
#include <memory>
#include <cstdint>
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

int main() {
    int num_images = 2;
    const char *filename = "../sim_image.png"; 
    int image_width, image_height, image_channels;
    unsigned char *image_data = stbi_load(filename, &image_width, &image_height, &image_channels, STBI_rgb_alpha);
    unsigned int image_size = image_height * image_width * image_channels;
    unsigned int batch_size = (image_size + IMAGE_METADATA_SIZE) * num_images;

    // Create an instance of ImageBatch
    ImageBatch batch;
    batch.height = image_height;
    batch.width = image_width;
    batch.channels = image_channels;
    batch.num_images = num_images;
    batch.batch_size = batch_size;
    batch.pipeline_id = 1;

    u_int bufferSize = image_size + IMAGE_METADATA_SIZE;
    unsigned char* total_buffer = new unsigned char[bufferSize*num_images];

    for(size_t i = 0; i < num_images; i++){
        unsigned int offset = i*bufferSize;

        std::memcpy((void*)(&total_buffer[offset]), &image_size, sizeof(unsigned int));
        std::memcpy((void*)(&total_buffer[offset+IMAGE_METADATA_SIZE]), image_data, image_size * sizeof(unsigned char));
    }

    batch.data = total_buffer;

    // Call SendImage to receive image information
    u_int16_t * error;
    MessageQueue* mq;
    if(mq->SendImage(batch, error)){
        std::cout << "Sending image was successful" << std::endl;
        // Output the received image information
        std::cout << "Received Image Information:" << std::endl;
        std::cout << "Width: " << batch.width << std::endl;
        std::cout << "Height: " << batch.height << std::endl;
        std::cout << "Channels: " << batch.channels << std::endl;
    } else {
        std::cout << "Sending image was unsuccessful" << std::endl;
        // Handle the error
        std::cerr << "Error occurred: " << error << std::endl;
    }

    return 0;
}
