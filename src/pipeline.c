#include <stdlib.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "types.h"

// Error codes
#define SUCCESS 0
#define FAILURE -1

void initializePipeline(Pipeline *pipeline, ProcessFunction *funcs, size_t size) {
    pipeline->functions = malloc(size * sizeof(ProcessFunction));
    memcpy(pipeline->functions, funcs, size * sizeof(ProcessFunction));
    pipeline->size = size;
}

int executeModuleInProcess(ProcessFunction func, int input, int *outputPipe) {
    // Create a new process
    pid_t pid = fork();

    if (pid == 0) {
        // Child process: Execute the module function
        int result = func(input);
        write(outputPipe[1], &result, sizeof(result)); // Write the result to the pipe
        exit(EXIT_SUCCESS);
    } else {
        // Parent process: Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            // Child process exited normally
            if (WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Child process exited with non-zero status\n");
                exit(EXIT_FAILURE);
            }
        } else {
            // Child process did not exit normally
            fprintf(stderr, "Child process did not exit normally\n");
            return FAILURE;
        }

        return SUCCESS;
    }
}

int executePipeline(Pipeline *pipeline, Data *data) {
    int outputPipe[2]; // Pipe for inter-process communication
    pipe(outputPipe);

    for (size_t i = 0; i < pipeline->size; ++i) {
        ProcessFunction func = pipeline->functions[i];

        int module_status = executeModuleInProcess(func, data->value, outputPipe);

        if (module_status == FAILURE) {
            close(outputPipe[0]); // Close the read end of the pipe
            close(outputPipe[1]); // Close the write end of the pipe
            return i + 1;
        }

        // Read the result from the pipe
        read(outputPipe[0], &data->value, sizeof(data->value));
    }

    close(outputPipe[0]); // Close the read end of the pipe
    close(outputPipe[1]); // Close the write end of the pipe

    return SUCCESS;
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

void main(int argc, char *argv[]) {
    int functionLimit = 100;
    void* functionPointers[functionLimit];
    int moduleReps = atoi(argv[2]);

    for (size_t i = 0; i < moduleReps; ++i) {
        functionPointers[i] = loadFunction("pi");
    }
    // Load modules from the configuration file
    // int numModules = loadModulesFromFile("modules.txt", functionPointers, functionLimit);

    // Initialize the pipeline
    Pipeline pipeline;
    initializePipeline(&pipeline, functionPointers, moduleReps);

    // Prepare the data
    Data data;

    // ... Initialize data ...
    data.value = atoi(argv[1]);

    // Execute the pipeline with parameter values
    int status = executePipeline(&pipeline, &data);

    if (status != SUCCESS) {
        // Print failure message
        printf("Failure in the pipeline\n");
        return;
    }

    // Print resulting data
    printf("Resulting data value: %d\n", data.value);

    // Clean up
    free(pipeline.functions);
}