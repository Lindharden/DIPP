#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include "types.h"

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
        data->value = func(data->value);
    }
}

// Define the expected function signature
typedef int (*ExpectedSignature)(int);

// Define a function that loads a function from a shared object and returns a function pointer
void* loadFunction(const char* moduleName) {
    char filename[256]; // Adjust the buffer size as needed
    snprintf(filename, sizeof(filename), "./external_modules/%s.so", moduleName);

    // Load the external library dynamically
    void* handle = dlopen(filename, RTLD_LAZY);
    if (handle == NULL) {
        fprintf(stderr, "Error: Unable to load the library %s.\n", filename);
        return NULL;
    }

    // Get a function pointer to the external function
    const char* run = "run";
    void* functionPointer = dlsym(handle, run);
    if (functionPointer == NULL) {
        fprintf(stderr, "Error: Unable to find the function %s in %s.\n", run, filename);
        dlclose(handle);
        return NULL;
    }

    // Verify the function signature
    if (sizeof(ExpectedSignature) != sizeof(functionPointer) || (ExpectedSignature)functionPointer != functionPointer) {
        fprintf(stderr, "Error: Function signature does not match the expected signature.\n");
        dlclose(handle);
        return NULL;
    }

    return functionPointer;
}

// Function to load modules from a configuration file
int loadModulesFromFile(const char* configFile, void* functionPointers[], int maxModules) {
    // Open the configuration file
    FILE* file = fopen(configFile, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open the configuration file.\n");
        return 0; // Return 0 to indicate failure
    }

    // Read module names from the configuration file and load them
    char line[256]; // Adjust the buffer size as needed
    int numModules = 0;
    
    while (fgets(line, sizeof(line), file) != NULL && numModules < maxModules) {
        // Remove the newline character, if present
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        // Load the module and store the function pointer
        functionPointers[numModules] = loadFunction(line);
        if (functionPointers[numModules] != NULL) {
            numModules++;
        }
    }

    // Close the configuration file
    fclose(file);

    return numModules;
}

int main(int argc, char *argv[]) {
    // TODO: Expand modules such that the inputs and outputs of each module can be different.

    int functionLimit = 10;
    void* functionPointers[functionLimit];

    // Load modules from the configuration file
    int numModules = loadModulesFromFile("modules.txt", functionPointers, functionLimit);

    // Initialize the pipeline
    Pipeline pipeline;
    initializePipeline(&pipeline, functionPointers, numModules);
    
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
