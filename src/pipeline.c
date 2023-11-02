#include <stdlib.h>
#include "types.h"
#include "modules/module_addition.c"
#include "modules/module_multiplication.c"
#include "modules/module_subtraction.c"

void initializePipeline(Pipeline *pipeline, ProcessFunction funcs[], size_t size) {
    pipeline->functions = malloc(size * sizeof(ProcessFunction));
    if (pipeline->functions == NULL) {
        // Handle memory allocation failure
    }
    memcpy(pipeline->functions, funcs, size * sizeof(ProcessFunction));
    pipeline->size = size;
}

void executePipeline(Pipeline *pipeline, Data *data) {
    for (size_t i = 0; i < pipeline->size; ++i) {
        ProcessFunction func = pipeline->functions[i];
        func(data);
    }
}

int main(int argc, char *argv[]) {

    // Define the processing functions to use
    ProcessFunction funcs[] = {additionModule, multiplicationModule, subtractionModule};
    
    // Initialize the pipeline
    Pipeline pipeline;
    initializePipeline(&pipeline, funcs, 3);
    
    // Prepare the data
    Data data;
    
    // ... Initialize data ...
    data.value = atoi(argv[1]);
    
    // Execute the pipeline
    executePipeline(&pipeline, &data);

    // Print resulting data
    printf("Resulting data value: %d\n", data.value);
    
    // Clean up
    free(pipeline.functions);
    
    return 0;
}
