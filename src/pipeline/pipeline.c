#include <stdlib.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <param/param.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "pipeline.h"
#include "module_config.pb-c.h"
#include "pipeline_config.pb-c.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

size_t get_param_buffer(uint8_t **out, param_t *param)
{
    // initialize buffer for module parameters
    int initial_buf_size = DATA_PARAM_SIZE;
    uint8_t buf[initial_buf_size];
    param_get_data(param, buf, initial_buf_size);
    int buf_size = (int)buf[0];

    *out = malloc(buf_size * sizeof(uint8_t));
    if (!*out)
    {
        return 0;
    }

    // Copy the data from the original buffer to the new buffer
    for (size_t i = 0; i < buf_size; i++)
    {
        (*out)[i] = buf[i + 1];
    }

    return buf_size;
}

// Function to load a module and parameter from a configuration file
void *load_module(char *moduleName)
{
    char filename[256]; // Adjust the buffer size as needed
    snprintf(filename, sizeof(filename), "/usr/share/pipeline/%s.so", moduleName);

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

void setup_pipeline(param_t *param, int index)
{
    uint8_t *buffer = NULL;
    size_t buf_size = get_param_buffer(&buffer, param);

    PipelineDefinition *pdef = pipeline_definition__unpack(NULL, buf_size, buffer);

    free(buffer);
    buffer = NULL;

    if (!pdef)
    {
        return; // Skip this pipeline if unpacking fails
    }

    int pipeline_id = param->id - PIPELINE_PARAMID_OFFSET;
    pipelines[pipeline_id].pipeline_id = pipeline_id + 1;
    pipelines[pipeline_id].num_modules = pdef->n_modules;

    for (size_t module_idx = 0; module_idx < pdef->n_modules; module_idx++)
    {
        ModuleDefinition *mdef = pdef->modules[module_idx];
        pipelines[pipeline_id].modules[module_idx].module_name = strdup(mdef->name);
        pipelines[pipeline_id].modules[module_idx].module_function = load_module(mdef->name);
        pipelines[pipeline_id].modules[module_idx].module_param_id = mdef->param_id - 1;
    }
}

void setup_module_config(param_t *param, int index)
{
    uint8_t *buffer = NULL;
    size_t buf_size = get_param_buffer(&buffer, param);

    ModuleConfig *mcon = module_config__unpack(NULL, buf_size, buffer);

    free(buffer);
    buffer = NULL;

    if (!mcon)
    {
        return; // Skip this module if unpacking fails
    }

    int module_id = param->id - MODULE_PARAMID_OFFSET; // Minus 30 cause IDs are offset by 30 to accommodate pipeline ids (see pipeline.h)
    module_parameter_lists[module_id].n_parameters = mcon->n_parameters;
    module_parameter_lists[module_id].parameters = malloc(mcon->n_parameters * sizeof(ModuleParameter *));
    for (size_t i = 0; i < mcon->n_parameters; i++)
    {
        module_parameter_lists[module_id].parameters[i] = malloc(sizeof(ModuleParameter));
        module_parameter_lists[module_id].parameters[i]->key = mcon->parameters[i]->key;
        module_parameter_lists[module_id].parameters[i]->value_case = mcon->parameters[i]->value_case;

        switch (mcon->parameters[i]->value_case)
        {
        case CONFIG_PARAMETER__VALUE_BOOL_VALUE:
            module_parameter_lists[module_id].parameters[i]->bool_value = mcon->parameters[i]->bool_value;
            break;
        case CONFIG_PARAMETER__VALUE_INT_VALUE:
            module_parameter_lists[module_id].parameters[i]->int_value = mcon->parameters[i]->int_value;
            break;
        case CONFIG_PARAMETER__VALUE_FLOAT_VALUE:
            module_parameter_lists[module_id].parameters[i]->float_value = mcon->parameters[i]->float_value;
            break;
        case CONFIG_PARAMETER__VALUE_STRING_VALUE:
            module_parameter_lists[module_id].parameters[i]->string_value = mcon->parameters[i]->string_value;
            break;
        default:
            break;
        }
    }
}

void setup_all_pipelines()
{
    for (size_t pipeline_idx = 0; pipeline_idx < MAX_PIPELINES; pipeline_idx++)
    {
        setup_pipeline(pipeline_config_params[pipeline_idx], 0);
    }
}

void setup_all_module_configs()
{
    for (size_t module_idx = 0; module_idx < MAX_MODULES; module_idx++)
    {
        setup_module_config(module_config_params[module_idx], 0);
    }
}

int execute_module_in_process(ProcessFunction func, ImageBatch *input, int *outputPipe, ModuleParameterList *config)
{
    // Create a new process
    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process: Execute the module function
        ImageBatch result = func(input, config);
        size_t data_size = sizeof(result);
        write(outputPipe[1], &result, data_size); // Write the result to the pipe
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
                return FAILURE;
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

int execute_pipeline(Pipeline *pipeline, ImageBatch *data)
{
    int outputPipe[2]; // Pipe for inter-process communication
    pipe(outputPipe);

    for (size_t i = 0; i < pipeline->num_modules; ++i)
    {
        ProcessFunction module_function = pipeline->modules[i].module_function;
        ModuleParameterList *module_config = &module_parameter_lists[pipeline->modules[i].module_param_id];

        int module_status = execute_module_in_process(module_function, data, outputPipe, module_config);

        if (module_status == FAILURE)
        {
            close(outputPipe[0]); // Close the read end of the pipe
            close(outputPipe[1]); // Close the write end of the pipe
            return i + 1;
        }

        ImageBatch result;
        read(outputPipe[0], &result, sizeof(result)); // Read the result from the pipe
        data->width = result.width;
        data->height = result.height;
        data->channels = result.channels;
        if (data->shm_key != result.shm_key)
        {
            // Recieve shared memory id from result data
            int shmid;
            if ((shmid = shmget(result.shm_key, 0, 0)) == -1)
            {
                perror("Could not get shared memory");
            }

            // Attach to shared memory from id
            void *shmaddr = shmat(shmid, NULL, 0);
            data->data = shmaddr;
        }
        else
        {
            data->data = result.data;
        }
        data->shm_key = result.shm_key;
        data->num_images = result.num_images;
        data->batch_size = result.batch_size;
        data->pipeline_id = result.pipeline_id;
    }

    close(outputPipe[0]); // Close the read end of the pipe
    close(outputPipe[1]); // Close the write end of the pipe

    return SUCCESS;
}

void save_images(const char *filename_base, const ImageBatch *batch)
{
    size_t offset = 0;
    int image_index = 0;
    
    while (image_index < batch->num_images && offset < batch->batch_size) {
        size_t image_size = *((size_t *)(batch->data + offset));
        offset += sizeof(size_t); // Move the offset to the start of the image data

        char filename[20];
        sprintf(filename, "%s%d.png", filename_base, image_index);

        int stride = batch->width * batch->channels * sizeof(uint8_t);
        int success = stbi_write_png(filename, batch->width, batch->height, batch->channels, batch->data + offset, stride);
        if (!success)
        {
            fprintf(stderr, "Error writing image to %s\n", filename);
        }
        else
        {
            printf("Image saved as %s\n", filename);
        }

        offset += image_size; // Move the offset to the start of the next image block

        image_index++;
    }
}

int get_pipeline_by_id(int pipeline_id, Pipeline **pipeline)
{
    for (size_t i = 0; i < MAX_PIPELINES; i++)
    {
        if (pipelines[i].pipeline_id == pipeline_id)
        {
            *pipeline = &pipelines[i];
            return SUCCESS;
        }
    }
    return FAILURE;
}

void process(ImageBatch *input_batch)
{
    // Recieve shared memory id from recieved data
    int shmid;
    if ((shmid = shmget(input_batch->shm_key, 0, 0)) == -1)
    {
        perror("Could not get shared memory");
    }

    // Attach to shared memory from id
    void *shmaddr = shmat(shmid, NULL, 0);
    input_batch->data = shmaddr; // retrieve correct address in shared memory

    // Execute the pipeline with parameter values
    Pipeline *pipeline;
    if (get_pipeline_by_id(input_batch->pipeline_id, &pipeline) == FAILURE)
    {
        fprintf(stderr, "Pipeline with id '%d' does not exist.\n", input_batch->pipeline_id);
    }

    int status = execute_pipeline(pipeline, input_batch);

    if (status != SUCCESS)
    {
        // Print failure message
        printf("Module named '%s' caused a failure in the pipeline\n", pipeline->modules[status - 1].module_name);
        return;
    }

    save_images("output", input_batch);

    // Detach and free shared memory
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL);
}

void setup_cache_if_needed()
{
    if (!is_setup)
    {
        // Fetch and setup pipeline and module configurations if not done
        setup_all_pipelines();
        setup_all_module_configs();
        is_setup = 1;
    }
}

int get_message_from_queue(ImageBatch *datarcv, int do_wait)
{
    int msg_queue_id;
    if ((msg_queue_id = msgget(MSG_QUEUE_KEY, 0)) == -1)
    {
        perror("Could not get MSG queue");
        return FAILURE;
    }

    if (msgrcv(msg_queue_id, datarcv, sizeof(ImageBatch) - sizeof(long), 1, do_wait ? 0 : IPC_NOWAIT) == -1)
    {
        perror("Message queue");
        return FAILURE;
    }

    return SUCCESS;
}

/* Process one image batch from the message queue*/
void process_one(int do_wait)
{
    setup_cache_if_needed();
    ImageBatch datarcv;
    if (get_message_from_queue(&datarcv, do_wait) == FAILURE)
    {
        return;
    }
    process(&datarcv);
}

/* Process all image batches in the message queue*/
void process_all(int do_wait)
{
    setup_cache_if_needed();
    ImageBatch datarcv;
    while (get_message_from_queue(&datarcv, do_wait) == SUCCESS)
    {
        process(&datarcv);
    }
}

void callback_run(param_t *param, int index)
{       
    switch (param_get_uint8(param))
    {
    case PROCESS_ONE:
        process_one(0);
        break;
    case PROCESS_ALL:
        process_all(0);
        break;
    case PROCESS_WAIT_ONE:
        process_one(1);
        break;
    case PROCESS_WAIT_ALL:
        process_all(1);
        break;
    default:
        return;
    }

    // Turn off pipeline when finished
    param_set_uint8(&pipeline_run, 0);
}