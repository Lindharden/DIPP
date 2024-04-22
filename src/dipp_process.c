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
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <time.h>
#include "dipp_run.h"
#include "dipp_error.h"
#include "dipp_config.h"
#include "dipp_process.h"
#include "dipp_process_param.h"
#include "dipp_paramids.h"
#include "vmem_storage.h"
#include "vmem_upload.h"
#include "metadata.pb-c.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define BILLION 1000000000L // 1 billion nanoseconds in a second

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
    int error_pipe[2];  // Pipe for inter-process error communication
    pipe(output_pipe);
    pipe(error_pipe);
    int num = data->pipeline_id < 0 ? data->pipeline_id * (-1) : data->pipeline_id;
    for (size_t i = 0; i < num; ++i)
    {
        err_current_module = i + 1;
        ProcessFunction module_function = pipeline->modules[0].module_function;
        ModuleParameterList *module_config = &module_parameter_lists[pipeline->modules[0].module_param_id];

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
        if (res == 0)
        {
            set_error_param(PIPE_EMPTY);
            return FAILURE;
        }

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
    uint32_t offset = 0;
    int image_index = 0;

    while (image_index < batch->num_images && offset < batch->batch_size)
    {
        uint32_t meta_size = *((uint32_t *)(batch->data + offset));
        offset += sizeof(uint32_t); // Move the offset to the start of metadata
        Metadata *metadata = metadata__unpack(NULL, meta_size, batch->data + offset);
        offset += meta_size; // Move offset to start of image

        char filename[20];
        sprintf(filename, "%s%d.png", filename_base, image_index);

        int stride = metadata->width * metadata->channels * sizeof(uint8_t);
        int success = stbi_write_png(filename, metadata->width, metadata->height, metadata->channels, batch->data + offset, stride);
        if (!success)
        {
            fprintf(stderr, "Error writing image to %s\n", filename);
        }
        else
        {
            printf("Image saved as %s\n", filename);
        }

        offset += metadata->size; // Move the offset to the start of the next image block

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
    int id = input_batch->pipeline_id < 0 ? 2 : 1;
    // Default to 1, as we use pipeline id to indicate amount of modules.
    if (get_pipeline_by_id(id, &pipeline) == FAILURE)
        return FAILURE;

    err_current_pipeline = pipeline->pipeline_id;

    return execute_pipeline(pipeline, input_batch);
}

void process(ImageBatch *input_batch, int time)
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

    struct timespec start_time;
    if (time)
    {
        // Get the start time
        if (clock_gettime(CLOCK_MONOTONIC, &start_time) < 0)
        {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }
    }
    

    int pipeline_result = load_pipeline_and_execute(input_batch);

    if (time)
    {
        // Get the end time
        struct timespec stop_time;
        if (clock_gettime(CLOCK_MONOTONIC, &stop_time) < 0)
        {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }

        long execution_time = BILLION * (stop_time.tv_sec - start_time.tv_sec) + (stop_time.tv_nsec - start_time.tv_nsec);
        float throughput = (input_batch->num_images * BILLION) / (execution_time / input_batch->pipeline_id); // MB / sec
        printf("%d %d %ld %.2f\n", input_batch->pipeline_id, input_batch->num_images, execution_time, throughput);
    }

    // Reset err values
    err_current_pipeline = 0;
    err_current_module = 0;

    if (pipeline_result == SUCCESS)
    {
        // save_images("output", input_batch);
        // upload(input_batch->data, input_batch->batch_size);
    }

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

    if (msgrcv(msg_queue_id, datarcv, sizeof(ImageBatch), 1, do_wait ? 0 : IPC_NOWAIT) == -1)
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
        process(&datarcv, 0);
}

/* Process all image batches in the message queue*/
void process_all(int do_wait)
{
    setup_cache_if_needed();

    ImageBatch datarcv;
    while (get_message_from_queue(&datarcv, do_wait) == SUCCESS)
        process(&datarcv, 1);
}

typedef struct ProcessThreadArgs
{
    int all;
    int wait;
    param_t *param;
} ProcessThreadArgs;

atomic_int is_processing = ATOMIC_VAR_INIT(0);

void *process_thread(void *arg)
{
    ProcessThreadArgs *args = (ProcessThreadArgs *)arg;
    int all = args->all;
    int wait = args->wait;
    param_t *param = args->param;
    free(args);

    if (all)
        process_all(wait);
    else
        process_one(wait);

    /* Indicate that processing is finished */
    atomic_store(&is_processing, 0);
    param_set_uint8(param, 0);

    return NULL;
}

void callback_run(param_t *param, int index)
{
    uint8_t param_value = param_get_uint8(param);
    if (!param_value)
        return;

    /* Check whether a thread is currently processing */
    int expected = 0;
    if (!atomic_compare_exchange_strong(&is_processing, &expected, 1))
    {
        // another thread is already processing
        return;
    }

    /* Initialize thread variables */
    ProcessThreadArgs *args = malloc(sizeof(ProcessThreadArgs));
    if (args == NULL)
    {
        // atomically update is_processing
        atomic_store(&is_processing, 0);
        return;
    }

    args->all = param_value % 2 == 0;
    args->wait = param_value > 2;
    args->param = param;

    /* Execute pipeline on new thread, to allow callback to finish */
    pthread_t processing_thread;
    if (pthread_create(&processing_thread, NULL, process_thread, args) != 0)
    {
        // create thread failed
        free(args);
        atomic_store(&is_processing, 0);
        return;
    }

    pthread_detach(processing_thread);
}