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
meson setup . builddir
ninja -C builddir
```
or simply execute the script `configure`.

## Run the Pipeline
Run the `dipp` binary from the `builddir` directory using `./builddir/dipp [OPTIONS]`. The `dipp` binary supports the following arguments:

| **Flag** | **Default value** | **Description**                                                             |
|----------|-------------------|-----------------------------------------------------------------------------|
| `-i`     | `ZMQ`             |  The connection interface to utilize for the pipeline. Possible values are: <br> - `ZMQ`: Run the pipeline with ZMQ (requires running ZMQ proxy). <br> - `KISS`: Run the pipeline with a defined KISS device. <br> - `CAN`: Run the pipeline with a defined CAN device. |
| `-p`     | `localhost`       | Port or device to use for the specified connection interface. Examples are: <br> - `/dev/ttyS1` (KISS) <br> - `vcan0` (CAN) |
| `-a`     | `162`             | CSP node address for the DIPP application.                                  |

The pipeline will act as a CSP/Param application.

## Pipeline data format
The pipeline processes batched image data that is stored in shared memory. The pipeline expects to receive metadata on the image batches through a System V Message Queue (ID: 71). The image batch metadata will be included in a `ImageBatch` struct of the following form:
```c
typedef struct ImageBatch {
    long mtype;          /* message type to read from the message queue */
    int num_images;      /* amount of images */
    int batch_size;      /* size of the image batch */
    int shm_key;         /* key to shared memory segment of image data */
    int pipeline_id;     /* id of pipeline to utilize for processing */
    unsigned char *data; /* address to image data (in shared memory) */
} ImageBatch;
```
Modules will receive and return image batches of this format.

## Camera simulator
For testing purposes a camera simulating program is included in the `sim` folder. Compile the program with the `compile` script.
Now run it with `./camera <pipeline_id>` (default pipeline_id = 1). To enqueue an image batch, type a positive integer indicating the desired number of images in the batch and hit enter. NB: place a PNG image in the `sim` folder and name it `sim_image.png`.

## Activate the pipeline
To activate the pipeline, utilize the `pipeline_run` parameter on the CSP node through CSH. Navigate to `node 162` (default port), and download the list of parameters using `list download`. Set the `pipeline_run` parameter to one of the following values:

| **_pipeline_run_ value** | **Batch processing procedure** | **Description**                                                                                         |
|--------------------------|--------------------------------|---------------------------------------------------------------------------------------------------------|
| 1                        | _PROCESS_ONE_                  | Process a single image batch.                                                                           |
| 2                        | _PROCESS_ALL_                  | Process all image batches in message queue.                                                             |
| 3                        | _PROCESS_WAIT_ONE_             | Wait for image batch to arrive in message queue. Then process one image batch.                          |
| 4                        | _PROCESS_WAIT_ALL_             | Wait for image batches to arrive in message queue. Then continously process all arriving image batches. |


## Error signaling
DIPP includes an integer parameter indicating the most recent cause of failure. The most resent error code can be accessed through the CSP parameter named `log_status`. The possible error codes, and their meaning, can be seen in the table below.

| Error Code | Description                                            |
|------------|--------------------------------------------------------|
| 100        | Memory Error: Malloc                                   |
| 101        | Memory Error: Realloc                                  |
| 102        | Memory Error: Free                                     |
| 200        | Message Queue Error: Not Found                         |
| 201        | Message Queue Error: Empty                             |
| 300        | Shared Memory Error: Not Found                         |
| 301        | Shared Memory Error: Detach                            |
| 302        | Shared Memory Error: Remove                            |
| 303        | Shared Memory Error: Attach                            |
| 400        | Pipe Error: Read                                       |
| 401        | Pipe Error: Empty                                      |
| 500        | Internal Error: PID Not Found                          |
| 501        | Internal Error: Shared Object Not Found                |
| 502        | Internal Error: Run Not Found                          |
| 503        | Internal Error: Boolean Parameter Not Found            |
| 504        | Internal Error: Integer Parameter Not Found            |
| 505        | Internal Error: Float Parameter Not Found              |
| 506        | Internal Error: String Parameter Not Found             |
| 507        | Internal Error: Custom metadata Boolean Not Found      |
| 508        | Internal Error: Custom metadata Integer Not Found      |
| 509        | Internal Error: Custom metadata Float Not Found        |
| 510        | Internal Error: Custom metadata String Not Found       |
| 511        | Internal Error: Custom metadata Dublicate Key Added    |
| 512        | Internal Error: Failed to pull remote parameter        |
| 513        | Internal Error: Failed to push remote parameter        |
| 514        | Internal Error: VMEM Not Found                         |
| 515        | Internal Error: VMEM Upload Failed                     |
| 600        | Module Exit Error: Crash                               |
| 601        | Module Exit Error: Normal                              |
| 700-799    | Module Exit Error: Custom error code defined by module |

All error codes are suffixed with ID of the pipeline (1 digit ranging from 1-9) and index of the module (2 digits ranging from 1-99) if applicable. Error codes suffixed with 0's indicate failure happening outside of a module.

### Error code examples
- _**708104**_: Module with index 4 in pipeline 1 failed with code 8.
- _**600212**_: Module with index 12 in pipeline 2 crashed.
- _**300205**_: Could not get Shared Memory in module 5 in pipeline 2.
- _**501000**_: Could not find SO file with name 'module' during preload.
