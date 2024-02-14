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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "types.h"
#include "pipeline.h"
#include "../param_config.h"
#include "../vmem_config.h"
#include "../protos/module_config.pb-c.h"
#include "../protos/pipeline_config.pb-c.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Error codes
#define SUCCESS 0
#define FAILURE -1

/* Define module specific parameters */
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_1, module_param_1, PARAM_TYPE_DATA, 200, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_1, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_2, module_param_2, PARAM_TYPE_DATA, 200, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_2, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_3, module_param_3, PARAM_TYPE_DATA, 200, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_3, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_4, module_param_4, PARAM_TYPE_DATA, 200, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_4, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_5, module_param_5, PARAM_TYPE_DATA, 200, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_5, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_6, module_param_6, PARAM_TYPE_DATA, 200, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_6, NULL);

void callback_run(param_t *param, int index)
{
    if (param_get_uint8(param) > 0)
    {
        run_pipeline();
        param_set_uint8(param, 0);
    }
}

PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG, pipeline_config, PARAM_TYPE_DATA, 200, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_PIPELINE, NULL);

static param_t *params[] = {&module_param_1, &module_param_2, &module_param_3, &module_param_4, &module_param_5, &module_param_6};

/* Define a pipeline_run parameter */
static uint8_t _pipeline_run = 0;
PARAM_DEFINE_STATIC_RAM(PARAMID_PIPELINE_RUN, pipeline_run, PARAM_TYPE_UINT8, -1, 0, PM_CONF, callback_run, NULL, &_pipeline_run, "Set the pipeline to execute the file");

void initialize_pipeline(Pipeline *pipeline, ProcessFunction *funcs, size_t size)
{
    pipeline->functions = malloc(size * sizeof(ProcessFunction));
    memcpy(pipeline->functions, funcs, size * sizeof(ProcessFunction));
    pipeline->size = size;
}

int execute_module_in_process(ProcessFunction func, ImageBatch *input, int *outputPipe, ModuleConfig *config)
{
    // Create a new process
    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process: Execute the module function
        ImageBatch result = func(input, config);
        size_t data_size = sizeof(result);
        write(outputPipe[1], &result, data_size);            // Write the result to the pipe
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Parent process: Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            // Child process exited normally
            if (WEXITSTATUS(status) != 0)
            {
                fprintf(stderr, "Child process exited with non-zero status\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // Child process did not exit normally
            fprintf(stderr, "Child process did not exit normally\n");
            return FAILURE;
        }

        return SUCCESS;
    }
}

int get_buf_size(uint8_t *buf, size_t buf_size)
{
    size_t actual_size = 0;

    while (actual_size < buf_size && buf[actual_size] != 0)
    {
        actual_size++;
    }

    return actual_size;
}

void trim_buffer(uint8_t *buf, uint8_t *old_buf, size_t buf_size)
{
    // Copy the data from the original buffer to the new buffer
    for (size_t i = 0; i < buf_size; i++)
    {
        buf[i] = old_buf[i];
    }
}

int execute_pipeline(Pipeline *pipeline, ImageBatch *data, int param_ids[])
{
    int outputPipe[2]; // Pipe for inter-process communication
    pipe(outputPipe);

    for (size_t i = 0; i < pipeline->size; ++i)
    {
        ProcessFunction func = pipeline->functions[i];

        // initialize buffer for module parameters
        int initial_buf_size = 200;
        uint8_t buf[initial_buf_size];
        param_get_data(params[param_ids[i] - 1], buf, initial_buf_size);
        int buf_size = get_buf_size(buf, initial_buf_size);

        // allocate trimmed buffer and copy data
        uint8_t trimmed_buf[buf_size];
        trim_buffer(trimmed_buf, buf, buf_size);

        // find specific value from key in protodata
        ModuleConfig *unpacked_config = module_config__unpack(NULL, buf_size, trimmed_buf);
        
        int module_status = execute_module_in_process(func, data, outputPipe, unpacked_config);

        if (module_status == FAILURE)
        {
            close(outputPipe[0]); // Close the read end of the pipe
            close(outputPipe[1]); // Close the write end of the pipe
            return i + 1;
        }

        ImageBatch result;
        read(outputPipe[0], &result, sizeof(result)); // Read the result from the pipe
        data->channels = result.channels;
        data->width = result.width;
        data->height = result.height;
        data->shm_key = result.shm_key;
        data->num_images = result.num_images;
        data->data = result.data;
    }

    close(outputPipe[0]); // Close the read end of the pipe
    close(outputPipe[1]); // Close the write end of the pipe

    return SUCCESS;
}

// Function to parse a configuration file with module and parameter names
int unpack_configurations(char *modules[], int param_ids[])
{
     // initialize buffer for module parameters
    int initial_buf_size = 200;
    uint8_t buf[initial_buf_size];
    param_get_data(&pipeline_config, buf, initial_buf_size);
    int buf_size = get_buf_size(buf, initial_buf_size);

    // allocate trimmed buffer and copy data
    uint8_t trimmed_buf[buf_size];
    trim_buffer(trimmed_buf, buf, buf_size);

    // get pipeline definition
    PipelineDefinition *unpacked_config = pipeline_definition__unpack(NULL, buf_size, trimmed_buf);

    // save module name and param id for each module definition
    for (size_t i = 0; i < unpacked_config->n_modules; i++) {
        ModuleDefinition *unpacked_definition = unpacked_config->modules[i];
        // the 'order' fields should be incremental
        modules[unpacked_definition->order - 1] = unpacked_definition->name;
        param_ids[unpacked_definition->order - 1] = unpacked_definition->param_id;
    }

    return unpacked_config->n_modules;
}

// Function to load a module and parameter from a configuration file
void *load_module(char *moduleName)
{
    char filename[256]; // Adjust the buffer size as needed
    snprintf(filename, sizeof(filename), "./external_modules/%s.so", moduleName);

    // Load the external library dynamically
    void *handle = dlopen(filename, RTLD_LAZY);
    if (handle == NULL)
    {
        fprintf(stderr, "Error: Unable to load the library %s.\n", filename);
        return NULL;
    }

    // Get a function pointer to the external function
    void *functionPointer = dlsym(handle, "run");
    if (functionPointer == NULL)
    {
        fprintf(stderr, "Error: Unable to find the run function in %s.\n", filename);
        dlclose(handle);
        return NULL;
    }

    return functionPointer;
}

// Function to load modules from a configuration file
int load_modules_with_params(void *functionPointers[], char *modules[], int param_ids[])
{
    int numModules = unpack_configurations(modules, param_ids);

    for (int i = 0; i < numModules; ++i)
    {
        // Load the module with the associated parameter
        functionPointers[i] = load_module(modules[i]);

        if (functionPointers[i] == NULL)
        {
            // Handle loading failure

            fprintf(stderr, "Error: Unable to load module %s.\n", modules[i]);
        }
    }

    return numModules;
}

void check_run(void)
{
    // run the pipeline if the _pipeline_run parameter is set w
    uint8_t do_run = param_get_uint8(&pipeline_run);
    if (do_run > 0)
    {
        run_pipeline();
        param_set_uint8(&pipeline_run, 0);
    }
}

void pipeline_configurations()
{
    // Pipeline configuration example (should be defined as a generic CSH method)
    PipelineDefinition pipeline_definition = PIPELINE_DEFINITION__INIT;
    ModuleDefinition flip_module = MODULE_DEFINITION__INIT;
    flip_module.order = 1;
    flip_module.name = "flip_horizontally";
    flip_module.param_id = 1;
    pipeline_definition.modules = malloc(sizeof(ModuleDefinition *));
    pipeline_definition.n_modules = 1;
    pipeline_definition.modules[0] = &flip_module;
    size_t lenConfig = pipeline_definition__get_packed_size(&pipeline_definition);
    uint8_t bufConfig[lenConfig];
    pipeline_definition__pack(&pipeline_definition, bufConfig);
    param_set_data(&pipeline_config, bufConfig, lenConfig);
    free(pipeline_definition.modules); 
}

void module_configurations()
{
    // Module parameter config example (should be defined as a generic CSH method)
    ModuleConfig flip_config = MODULE_CONFIG__INIT;
    ConfigParameter flip_percent = CONFIG_PARAMETER__INIT;
    ConfigParameter flip_amount = CONFIG_PARAMETER__INIT;
    flip_percent.key = "flip_percent";
    flip_percent.value_case = CONFIG_PARAMETER__VALUE_FLOAT_VALUE;
    flip_percent.float_value = 0.20;
    flip_amount.key = "flip_amount";
    flip_amount.value_case = CONFIG_PARAMETER__VALUE_INT_VALUE;
    flip_amount.int_value = 1;
    flip_config.parameters = malloc(sizeof(ConfigParameter *) * 2);
    flip_config.n_parameters = 2;
    flip_config.parameters[0] = &flip_percent;
    flip_config.parameters[1] = &flip_amount;
    size_t lenConfig = module_config__get_packed_size(&flip_config);
    uint8_t bufConfig[lenConfig];
    module_config__pack(&flip_config, bufConfig);
    param_set_data(&module_param_1, bufConfig, lenConfig);
    free(flip_config.parameters);
}

void save_image(const char *filename, const ImageBatch *batch)
{
    // Determine the desired output format (e.g., PNG)
    int stride = batch->width * batch->channels;
    int success = stbi_write_png(filename, batch->width, batch->height, batch->channels, batch->data, stride);
    if (!success)
    {
        fprintf(stderr, "Error writing image to %s\n", filename);
    }
    else
    {
        printf("Image saved as %s\n", filename);
    }
}

void run_pipeline(void)
{

    pipeline_configurations();
    module_configurations();

    int functionLimit = 10;
    void *functionPointers[functionLimit];
    char *modules[functionLimit]; // Array to store the module names
    int param_ids[functionLimit];    // Array to store the parameters for the modules

    // Load modules from the configuration file
    int numModules = load_modules_with_params(functionPointers, modules, param_ids);

    // Initialize the pipeline
    Pipeline pipeline;
    initialize_pipeline(&pipeline, functionPointers, numModules);

    // Create msg queue
    int msg_queue_id;
    int MSG_QUEUE_KEY = 68;
    if ((msg_queue_id = msgget(MSG_QUEUE_KEY, 0)) == -1)
    {
        printf("Could not get MSG queue");
        perror("Could not get MSG queue");
    }

    // Check if there are messages in the queue
    struct msqid_ds buf;
    if (msgctl(msg_queue_id, IPC_STAT, &buf) == -1) {
        printf("msgctl error");
        perror("msgctl error");
    }

    if (buf.msg_qnum <= 0) {
        printf("No items in the msg queue");
        perror("No items in the msg queue");
        goto cleanup;
    }

    // Recieve msg from queue
    ImageBatch datarcv;
    if (msgrcv(msg_queue_id, &datarcv, sizeof(ImageBatch) - sizeof(long), 1, 0) == -1)
    {
        printf("msgrcv error");
        perror("msgrcv error");
    }

    // Recieve shared memory id from recieved data
    int shmid;
    if ((shmid = shmget(datarcv.shm_key, 0, 0)) == -1) 
    {
        printf("Could not get shared memory");
        perror("Could not get shared memory");
    }

    // Attach to shared memory from id
    int *shmaddr = shmat(shmid, NULL, 0);
    datarcv.data = shmaddr; // retrieve correct address in shared memory

    // Execute the pipeline with parameter values
    int status = execute_pipeline(&pipeline, &datarcv, param_ids);
    
    if (status != SUCCESS)
    {
        // Print failure message
        printf("Module named '%s' caused a failure in the pipeline\n", modules[status - 1]);
        return;
    }

    save_image("output.png", &datarcv);

    // Detach and free shared memory
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL);

cleanup:
    // Clean up functions
    free(pipeline.functions);
}
