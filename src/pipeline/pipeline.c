#include <stdlib.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <param/param.h>
#include <csp/csp.h>
#include "types.h"
#include "pipeline.h"
#include "../param_config.h"
#include "../vmem_config.h"

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

static param_t* params[] = {&module_param_1, &module_param_2, &module_param_3, &module_param_4, &module_param_5, &module_param_6};

/* Define a pipeline_run parameter */
static uint8_t _pipeline_run = 0;
PARAM_DEFINE_STATIC_RAM(PARAMID_PIPELINE_RUN, pipeline_run, PARAM_TYPE_UINT8, -1, 0, PM_CONF, NULL, NULL, &_pipeline_run, "Set the pipeline to execute the file");

void initializePipeline(Pipeline *pipeline, ProcessFunction *funcs, size_t size) {
    pipeline->functions = malloc(size * sizeof(ProcessFunction));
    if (pipeline->functions == NULL)
    {
        // Handle memory allocation failure
    }
    
    memcpy(pipeline->functions, funcs, size * sizeof(ProcessFunction));
    pipeline->size = size;
}

void executePipeline(Pipeline *pipeline, Data *data, int values[], int numModules) {
    for (size_t i = 0; i < pipeline->size; ++i) {
        ProcessFunction func = pipeline->functions[i];

        // Get the parameter value using param_get_uint8
        uint8_t paramValue = param_get_uint8(params[values[i] - 1]);

        // Pass the parameter value along with data->value to the function
        data->value = func(data->value, paramValue);
    }
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
        char output[100];
        sprintf(output, "%d", run_pipeline());
        csp_print(output);
        param_set_uint8(&pipeline_run, 0);
    }
}

int run_pipeline(void) {
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
    executePipeline(&pipeline, &data, values, numModules);

    // Print resulting data
    printf("Resulting data value: %d\n", data.value);

    // Clean up
    free(pipeline.functions);

    return data.value;
}
