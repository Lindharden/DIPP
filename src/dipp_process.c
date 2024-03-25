#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <param/param.h>
#include "dipp_error.h"
#include "dipp_config.h"
#include "dipp_process.h"
#include "dipp_process_param.h"
#include "dipp_paramids.h"
#include "vmem_storage.h"
#include "vmem_upload.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int execute_module_in_process(ProcessFunction func, ImageBatch *input, int *output_pipe, int *error_pipe, ModuleParameterList *config)
{
    // Create a new process
    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process: Execute the module function
        ImageBatch result = func(input, config, error_pipe);
        size_t data_size = sizeof(result);
        write(output_pipe[1], &result, data_size); // Write the result to the pipe
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Parent process: Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            // Child process exited normally (EXIT_FAILURE)
            if (WEXITSTATUS(status) != 0)
            {
                uint16_t module_error;
                size_t res = read(error_pipe[0], &module_error, sizeof(uint16_t));
                if (res == FAILURE)
                    set_error_param(PIPE_READ);
                else if (res == 0)
                    set_error_param(MODULE_EXIT_NORMAL);    
                else if (module_error < 100)
                    set_error_param(MODULE_EXIT_CUSTOM + module_error);
                else
                    set_error_param(module_error);
            
                fprintf(stderr, "Child process exited with non-zero status\n");
                return FAILURE;
            }
        }
        else
        {
            // Child process did not exit normally (CRASH)
            set_error_param(MODULE_EXIT_CRASH);
            fprintf(stderr, "Child process did not exit normally\n");
            return FAILURE;
        }

        return SUCCESS;
    }
}

int execute_pipeline(Pipeline *pipeline, ImageBatch *data)
{
    int output_pipe[2]; // Pipe for inter-process result communication
    int error_pipe[2]; // Pipe for inter-process error communication
    pipe(output_pipe);
    pipe(error_pipe);

    for (size_t i = 0; i < pipeline->num_modules; ++i)
    {
        err_current_module = i + 1;
        ProcessFunction module_function = pipeline->modules[i].module_function;
        ModuleParameterList *module_config = &module_parameter_lists[pipeline->modules[i].module_param_id];

        int module_status = execute_module_in_process(module_function, data, output_pipe, error_pipe, module_config);

        if (module_status == FAILURE)
        {
            /* Close all active pipes */
            close(output_pipe[0]); // Close the read end of the pipe
            close(output_pipe[1]); // Close the write end of the pipe
            close(error_pipe[0]);
            close(error_pipe[1]);
            return FAILURE;
        }

        ImageBatch result;
        int res = read(output_pipe[0], &result, sizeof(result)); // Read the result from the pipe
        if (res == FAILURE)
        {
            set_error_param(PIPE_READ);
            return FAILURE;
        }
        if (res == 0) {
            set_error_param(PIPE_EMPTY);
            return FAILURE;
        }
        data->width = result.width;
        data->height = result.height;
        data->channels = result.channels;
        if (data->shm_key != result.shm_key)
        {
            // Recieve shared memory id from result data
            int shmid;
            if ((shmid = shmget(result.shm_key, 0, 0)) == -1)
            {
                set_error_param(SHM_NOT_FOUND);
                perror("Could not get shared memory");
            }

            // Attach to shared memory from id
            void *shmaddr = shmat(shmid, NULL, 0);

            if (shmaddr == (void *)-1) 
            {
                set_error_param(SHM_ATTACH);
                perror("Could not attach to shared memory");
            }
            
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

    close(output_pipe[0]); // Close the read end of the pipe
    close(output_pipe[1]); // Close the write end of the pipe
    close(error_pipe[0]);
    close(error_pipe[1]);

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
    set_error_param(INTERNAL_PID_NOT_FOUND);
    return FAILURE;
}

int load_pipeline_and_execute(ImageBatch *input_batch)
{
    // Execute the pipeline with parameter values
    Pipeline *pipeline;
    if (get_pipeline_by_id(input_batch->pipeline_id, &pipeline) == FAILURE)
        return FAILURE;

    err_current_pipeline = pipeline->pipeline_id;

    return execute_pipeline(pipeline, input_batch);
}

void process(ImageBatch *input_batch)
{
    // Recieve shared memory id from recieved data
    int shmid;
    if ((shmid = shmget(input_batch->shm_key, 0, 0)) == FAILURE)
    {
        set_error_param(SHM_NOT_FOUND);
        return;
    }

    // Attach to shared memory from id
    void *shmaddr = shmat(shmid, NULL, 0);

    if (shmaddr == (void *)-1)
    {
        set_error_param(SHM_ATTACH);
        return;
    }

    input_batch->data = shmaddr; // retrieve correct address in shared memory

    if (load_pipeline_and_execute(input_batch) == SUCCESS)
    {
        save_images("output", input_batch);
        upload(input_batch->data, input_batch->batch_size);
    }        
    
    // Reset err values
    err_current_pipeline = 0;
    err_current_module = 0;

    // Detach and free shared memory
    if (shmdt(shmaddr) == -1)
    {
        set_error_param(SHM_DETACH);
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        set_error_param(SHM_REMOVE);
    }
}

int get_message_from_queue(ImageBatch *datarcv, int do_wait)
{
    int msg_queue_id;
    if ((msg_queue_id = msgget(MSG_QUEUE_KEY, 0)) == -1)
    {
        set_error_param(MSGQ_NOT_FOUND);
        return FAILURE;
    }

    if (msgrcv(msg_queue_id, datarcv, sizeof(ImageBatch) - sizeof(long), 1, do_wait ? 0 : IPC_NOWAIT) == -1)
    {
        set_error_param(MSGQ_EMPTY);
        return FAILURE;
    }

    return SUCCESS;
}

/* Process one image batch from the message queue*/
void process_one(int do_wait)
{
    setup_cache_if_needed();

    ImageBatch datarcv;
    if (get_message_from_queue(&datarcv, do_wait) == SUCCESS)
        process(&datarcv);
}

/* Process all image batches in the message queue*/
void process_all(int do_wait)
{
    setup_cache_if_needed();

    ImageBatch datarcv;
    while (get_message_from_queue(&datarcv, do_wait) == SUCCESS)
        process(&datarcv);
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
    param_set_uint8(param, 0);
}
