#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include "types.h"
#include "modules/module_addition.c"
#include "modules/module_multiplication.c"
#include "modules/module_subtraction.c"

void initializePipeline(Pipeline *pipeline, ProcessFunction *funcs, size_t size) {
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

// Define a function that loads a function from a shared object and returns a function pointer
void* loadFunction(const char* filename, const char* functionName) {
    // Load the external library dynamically
    void* handle = dlopen(filename, RTLD_LAZY);
    if (handle == NULL) {
        fprintf(stderr, "Error: Unable to load the library %s.\n", filename);
        return NULL;
    }

    // Get a function pointer to the external function
    void* functionPointer = dlsym(handle, functionName);
    if (functionPointer == NULL) {
        fprintf(stderr, "Error: Unable to find the function %s in %s.\n", functionName, filename);
        dlclose(handle);
        return NULL;
    }

    return functionPointer;
}

// Define a function that loads multiple functions from shared objects and stores them in an array
void loadFunctionsFromFiles(const char* filenames[], const char* functionNames[], int numFiles, void* functionPointers[]) {
    for (int i = 0; i < numFiles; i++) {
        functionPointers[i] = loadFunction(filenames[i], functionNames[i]);
    }
}

// Define the expected signature
typedef int (*AddOneFunction)(int);

int main(int argc, char *argv[]) {

    // TODO: Read functions from config file instead
    const char* sharedObjectFiles[] = {"./external_modules/addOne.so", "./external_modules/subtractOne.so"};
    const char* functionNames[] = {"addOne", "subtractOne"};

    // Define an array to store function pointers
    void* functionPointers[sizeof(sharedObjectFiles) / sizeof(sharedObjectFiles[0])];

    // Load functions from the shared objects
    loadFunctionsFromFiles(sharedObjectFiles, functionNames, sizeof(sharedObjectFiles) / sizeof(sharedObjectFiles[0]), functionPointers);

    // Initialize the pipeline
    Pipeline pipeline; // find the amount of functions by dividing the size of the array, with the size of the first element
    initializePipeline(&pipeline, functionPointers, sizeof(functionPointers) / sizeof(functionPointers[0]));
    
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
