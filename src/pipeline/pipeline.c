#include <stdlib.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <param/param.h>
#include <csp/csp.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "types.h"
#include "pipeline.h"
#include "../param_config.h"
#include "../vmem_config.h"

// Error codes
#define SUCCESS 0
#define FAILURE -1

/* Define module specific parameters */
static uint8_t _module_param_1 = 1;
static uint8_t _module_param_2 = 1;
static uint8_t _module_param_3 = 1;
static uint8_t _module_param_4 = 1;
static uint8_t _module_param_5 = 1;
static uint8_t _module_param_6 = 1;
PARAM_DEFINE_STATIC_RAM(PARAMID_MODULE_PARAM_1, module_param_1, PARAM_TYPE_UINT8, -1, 0, PM_CONF, NULL, NULL, &_module_param_1, "Module parameter");
PARAM_DEFINE_STATIC_RAM(PARAMID_MODULE_PARAM_2, module_param_2, PARAM_TYPE_UINT8, -1, 0, PM_CONF, NULL, NULL, &_module_param_2, "Module parameter");
PARAM_DEFINE_STATIC_RAM(PARAMID_MODULE_PARAM_3, module_param_3, PARAM_TYPE_UINT8, -1, 0, PM_CONF, NULL, NULL, &_module_param_3, "Module parameter");
PARAM_DEFINE_STATIC_RAM(PARAMID_MODULE_PARAM_4, module_param_4, PARAM_TYPE_UINT8, -1, 0, PM_CONF, NULL, NULL, &_module_param_4, "Module parameter");
PARAM_DEFINE_STATIC_RAM(PARAMID_MODULE_PARAM_5, module_param_5, PARAM_TYPE_UINT8, -1, 0, PM_CONF, NULL, NULL, &_module_param_5, "Module parameter");
PARAM_DEFINE_STATIC_RAM(PARAMID_MODULE_PARAM_6, module_param_6, PARAM_TYPE_UINT8, -1, 0, PM_CONF, NULL, NULL, &_module_param_6, "Module parameter");

PARAM_DEFINE_STATIC_VMEM(9, config_str, PARAM_TYPE_STRING, 200 ,0, PM_CONF, NULL, NULL, config, VMEM_CONF_CONFIG, NULL);

static param_t* params[] = {&module_param_1, &module_param_2, &module_param_3, &module_param_4, &module_param_5, &module_param_6};

/* Define a pipeline_run parameter */
static uint8_t _pipeline_run = 0;
PARAM_DEFINE_STATIC_RAM(PARAMID_PIPELINE_RUN, pipeline_run, PARAM_TYPE_UINT8, -1, 0, PM_CONF, NULL, NULL, &_pipeline_run, "Set the pipeline to execute the file");

void initializePipeline(Pipeline *pipeline, ProcessFunction *funcs, size_t size) {
    pipeline->functions = malloc(size * sizeof(ProcessFunction));
    memcpy(pipeline->functions, funcs, size * sizeof(ProcessFunction));
    pipeline->size = size;
}

int executeModuleInProcess(ProcessFunction func, int input, int *outputPipe, uint8_t paramValue) {
    // Create a new process
    pid_t pid = fork();

    if (pid == 0) {
        // Child process: Execute the module function
        int result = func(input, paramValue);
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

int executePipeline(Pipeline *pipeline, Data *data, int values[]) {
    int outputPipe[2]; // Pipe for inter-process communication
    pipe(outputPipe);

    for (size_t i = 0; i < pipeline->size; ++i) {
        ProcessFunction func = pipeline->functions[i];

        // Get the parameter value using param_get_uint8
        uint8_t paramValue = param_get_uint8(params[values[i] - 1]);

        int module_status = executeModuleInProcess(func, data->value, outputPipe, paramValue);

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

// Function to parse a configuration file with module and parameter names
int parseConfigFile(const char *configFile, char* modules[], int values[], int maxModules) {
    FILE *file = fopen(configFile, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open the configuration file.\n");
        return 0; // Return 0 to indicate failure
    }

    int numModules = 0;
    char line[256]; // Adjust the buffer size as needed

    while (fgets(line, sizeof(line), file) != NULL && numModules < maxModules) {
        // Remove the newline character, if present
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // Split the line by ':'
        char *moduleName = strtok(line, ":");
        char *paramName = strtok(NULL, ":");

        if (moduleName != NULL && paramName != NULL) {
            // Store the module and parameter names
            modules[numModules] = strdup(moduleName);
            values[numModules] = paramName[strlen(paramName) - 1] - '0';

            numModules++;
        }
    }
    
    fclose(file);

    return numModules;
}

// Function to load a module and parameter from a configuration file
void *loadModuleWithParam(const char *moduleName, const char *paramName) {
    char filename[256]; // Adjust the buffer size as needed
    snprintf(filename, sizeof(filename), "./external_modules/%s.so", moduleName);

    // Load the external library dynamically
    void *handle = dlopen(filename, RTLD_LAZY);
    if (handle == NULL) {
        fprintf(stderr, "Error: Unable to load the library %s.\n", filename);
        return NULL;
    }

    // Get a function pointer to the external function
    const char *run = "run";
    void *functionPointer = dlsym(handle, run);
    if (functionPointer == NULL) {
        fprintf(stderr, "Error: Unable to find the function %s in %s.\n", run, filename);
        dlclose(handle);
        return NULL;
    }

    return functionPointer;
}

// Function to load modules from a configuration file
int loadModulesWithParams(const char *configFile, void *functionPointers[], char* modules[], int values[], int maxModules) {
    int numModules = parseConfigFile(configFile, modules, values, maxModules);

    for (int i = 0; i < numModules; ++i) {
        // Load the module with the associated parameter
        functionPointers[i] = loadModuleWithParam(modules[i], values[i]);

        if (functionPointers[i] == NULL) {
            // Handle loading failure
            fprintf(stderr, "Error: Unable to load module %s with parameter %s.\n", modules[i], values[i]);
        }
    }

    return numModules;
}

void check_run(void) {
    // run the pipeline if the _pipeline_run parameter is set w
    uint8_t do_run = param_get_uint8(&pipeline_run);
    if (do_run > 0)
    {
        run_pipeline();
        param_set_uint8(&pipeline_run, 0);
    }
}

void run_pipeline(void) {
    int functionLimit = 10;
    void *functionPointers[functionLimit];
    char* modules[functionLimit]; // Array to store the module names
    int values[functionLimit]; // Array to store the parameters for the modules

    // Load modules from the configuration file
    int numModules = loadModulesWithParams("modules.txt", functionPointers, modules, values, functionLimit);

    // Initialize the pipeline
    Pipeline pipeline;
    initializePipeline(&pipeline, functionPointers, numModules);

    // Prepare the data
    Data data;

    // ... Initialize data ...
    data.value = 1000000;

    // Execute the pipeline with parameter values
    int status = executePipeline(&pipeline, &data, values);

    if (status != SUCCESS) {
        // Print failure message
        printf("Module named '%s' caused a failure in the pipeline\n", modules[status - 1]);
        return;
    }

    // Print resulting data
    printf("Resulting data value: %d\n", data.value);

    // Clean up
    free(pipeline.functions);
}
