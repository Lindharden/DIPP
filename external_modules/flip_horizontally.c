#include <stdio.h>
#include <stdlib.h>
#include "../src/pipeline/types.h"
#include "../protos/module_config.pb-c.h"

// GENERIC PROTOBUF UTIL FUNCTIONS //

ConfigParameter *get_param(ModuleConfig *config, const char *name) {
    // find parameter
    ConfigParameter *found_parameter = NULL;
    for (size_t i = 0; i < config->n_parameters; i++)
    {
        if (strcmp(config->parameters[i]->key, name) == 0)
        {
            found_parameter = config->parameters[i];
            break;
        }
    }
    return found_parameter;
}

float get_param_float(ModuleConfig *config, const char *name)
{   
    ConfigParameter *found_parameter = get_param(config, name);

    // identify value type
    if (found_parameter == NULL || found_parameter->value_case != CONFIG_PARAMETER__VALUE_FLOAT_VALUE)
    {
        perror("Parameter not found or not of type float.");
    }
    
    return found_parameter->float_value;
}

int get_param_int(ModuleConfig *config, const char *name)
{   
    ConfigParameter *found_parameter = get_param(config, name);

    // identify value type
    if (found_parameter == NULL || found_parameter->value_case != CONFIG_PARAMETER__VALUE_INT_VALUE)
    {
        perror("Parameter not found or not of type int.");
    }
    
    return found_parameter->int_value;
}

ImageBatch run(const ImageBatch *input_batch, ModuleConfig *config) {
    ImageBatch mirrored_batch;
    mirrored_batch.mtype = input_batch->mtype;
    mirrored_batch.width = input_batch->width;
    mirrored_batch.height = input_batch->height;
    mirrored_batch.channels = input_batch->channels;
    mirrored_batch.shm_key = input_batch->shm_key;
    mirrored_batch.num_images = input_batch->num_images;
    mirrored_batch.data = input_batch->data;

    float flip_percent = get_param_float(config, "flip_percent");
    int flip_height = (int)(input_batch->height * flip_percent);
    int flip_amount = get_param_int(config, "flip_amount");

    // Calculate the size of the entire batch
    int data_size = input_batch->width * input_batch->height * input_batch->channels * input_batch->num_images;   
    unsigned char *new_data = (unsigned char *)malloc(data_size);

    // Calculate the size of each image in bytes
    size_t image_size = input_batch->height * input_batch->width * input_batch->channels;

    for(int j = 0; j < flip_amount; ++j) {
        // Mirror each image horizontally
        for (int i = 0; i < input_batch->num_images; ++i) {
            unsigned char *image_data = &mirrored_batch.data[i * image_size];
            for (int y = 0; y < input_batch->height; ++y) {
                for (int x = 0; x < input_batch->width / 2; ++x) {
                    // Swap pixel values horizontally
                    for (int c = 0; c < input_batch->channels; ++c) {
                        if (y > flip_height) {
                            new_data[(y * input_batch->width + x) * input_batch->channels + c] = 
                                image_data[(y * input_batch->width + x) * input_batch->channels + c];
                            new_data[(y * input_batch->width + (input_batch->width - 1 - x)) * input_batch->channels + c] =
                                image_data[(y * input_batch->width + (input_batch->width - 1 - x)) * input_batch->channels + c];
                        } else {
                            unsigned char temp = image_data[(y * input_batch->width + x) * input_batch->channels + c];
                            new_data[(y * input_batch->width + x) * input_batch->channels + c] =
                                image_data[(y * input_batch->width + (input_batch->width - 1 - x)) * input_batch->channels + c];
                            new_data[(y * input_batch->width + (input_batch->width - 1 - x)) * input_batch->channels + c] = temp;
                        }
                    }
                }
            }
        }
    }

    memcpy(input_batch->data, new_data, data_size);

    free(new_data);

    return mirrored_batch;
}