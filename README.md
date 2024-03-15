# C Data Processing Pipeline
DIPP stands for: DISCO Image Processing Pipeline.

DIPP is a modular image processing pipeline tailored to run on the DISCO-2 CubeSat.

## Pipeline modules
To write modules for the pipeline, checkout the [DISCO-2 module template](https://github.com/Lindharden/DISCO2-module-template).

Place the compiled modules in the `external_modules` folder.

## Configuring the Pipeline Modules
Configuration of pipeline stages and module parameters are to be done through a CSH extension. Specifically, parameters for specific modules are updated with the csh function `ippc module [options] <module-idx> <config-file>` and pipeline configuration is updated with the csh function `ippc pipeline [options] <pipeline-idx> <config-file>`. Checkout the [csp_ippc extension](https://github.com/Lindharden/csp_ippc) for more information. All configurations are persistently stored in vmem, and do not have to be rerun on startup.

## Build the Pipeline
To build the project run the following commands:
```
meson . builddir
cd builddir
ninja
```
or simply execute the script `configure`.

## Run the Pipeline
Run the `dipp` binary from the `builddir` directory using `./builddir/dipp <arg_1> <arg_2>`. The `dipp` binary supports the following arguments:

| **Arg 1 (Interface)** | **Arg 2 (device/port)** | **Description**                                                   |
|-----------------------|--------------------------|------------------------------------------------------------------|
| ZMQ                   | IP address of ZMQ proxy  | Run the pipeline with ZMQ (requires running ZMQ proxy).         |
| KISS                  | KISS device              | Run the pipeline with a defined KISS device.                     |
| CAN                   | CAN device               | Run the pipeline with a defined CAN device.                      |

The pipeline acts as a CSP/Param application with node id 162.

## Pipeline data format
The pipeline processes batched image data that is stored in shared memory. The pipeline expects to receive metadata on the image batches through a System V Message Queue (ID: 71). The image batch metadata will be included in a `ImageBatch` struct of the following form:
```c
typedef struct ImageBatch {
    long mtype;          /* message type to read from the message queue */
    int height;          /* height of images */
    int width;           /* width of images */
    int channels;        /* channels of images */
    int num_images;      /* amount of images */
    int shm_key;         /* key to shared memory segment of image data */
    int pipeline_id;     /* id of pipeline to utilize for processing */
    unsigned char *data; /* address to image data (in shared memory) */
} ImageBatch;
```
Modules will receive and return image batches of this format.

## Camera simulator
For testing purposes a camera simulating program is included in the `sim` folder. Compile the program with the following command:
```
gcc -o camera camera_control.h -lm
```
Now run it with `./camera`. To enqueue an image batch, type whatever and hit enter. NB: place a PNG image in the `sim` folder and name it `input.png`.

## Activate the pipeline
To activate the pipeline, utilize the `pipeline_run` parameter on the CSP node through CSH. Navigate to `node 162` (default port), and download the list of parameters using `list download`. Set the `pipeline_run` parameter to one of the following values:

| **_pipeline_run_ value** | **Batch processing procedure** | **Description**                                                                                         |
|--------------------------|--------------------------------|---------------------------------------------------------------------------------------------------------|
| 1                        | _PROCESS_ONE_                  | Process a single image batch.                                                                           |
| 2                        | _PROCESS_ALL_                  | Process all image batches in message queue.                                                             |
| 3                        | _PROCESS_WAIT_ONE_             | Wait for image batch to arrive in message queue. Then process one image batch.                          |
| 4                        | _PROCESS_WAIT_ALL_             | Wait for image batches to arrive in message queue. Then continously process all arriving image batches. |
