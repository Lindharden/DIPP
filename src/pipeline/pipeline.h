#include "../param_config.h"
#include "../vmem_config.h"
#include "module_config.pb-c.h"
#include <stdlib.h>
#include <stdint.h>

#define DATA_PARAM_SIZE 188

#define MAX_MODULES 20
#define MAX_PIPELINES 5

// Error codes
#define SUCCESS 0
#define FAILURE -1

typedef struct ImageBatch {
    long mtype;
    int height;
    int width;
    int channels;
    int num_images;
    int shm_key;
    char *pipeline_name; // which pipeline to utilize
    unsigned char *data; // batched image data (255 different values)
} ImageBatch;

typedef ImageBatch (*ProcessFunction)(ImageBatch *, ModuleConfig *);

typedef struct Module {
    char *module_name;
    ProcessFunction module_function;
    ModuleConfig *module_param;
} Module;

typedef struct Pipeline {
    char *pipeline_name;
    Module modules[MAX_MODULES];
    int num_modules;
} Pipeline;

/* Define variables for caching modules and pipelines */
static Pipeline pipelines[MAX_PIPELINES];

void setup();

/* Define module specific parameters */
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_1,  module_param_1,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_1,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_2,  module_param_2,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_2,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_3,  module_param_3,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_3,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_4,  module_param_4,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_4,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_5,  module_param_5,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_5,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_6,  module_param_6,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_6,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_7,  module_param_7,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_7,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_8,  module_param_8,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_8,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_9,  module_param_9,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_9,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_10, module_param_10, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_10, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_11, module_param_11, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_11, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_12, module_param_12, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_12, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_13, module_param_13, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_13, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_14, module_param_14, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_14, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_15, module_param_15, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_15, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_16, module_param_16, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_16, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_17, module_param_17, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_17, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_18, module_param_18, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_18, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_19, module_param_19, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_19, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_20, module_param_20, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_MODULE_20, NULL);
static param_t *module_configs[] = {
    &module_param_1,  &module_param_2,  &module_param_3,  &module_param_4,  &module_param_5, 
    &module_param_6,  &module_param_7,  &module_param_8,  &module_param_9,  &module_param_10, 
    &module_param_11, &module_param_12, &module_param_13, &module_param_14, &module_param_15, 
    &module_param_16, &module_param_17, &module_param_18, &module_param_19, &module_param_20 
};

/* Define pipeline configuration parameters */
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_1, pipeline_config_1, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_PIPELINE_1, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_2, pipeline_config_2, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_PIPELINE_2, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_3, pipeline_config_3, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_PIPELINE_3, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_4, pipeline_config_4, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_PIPELINE_4, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_5, pipeline_config_5, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_PIPELINE_5, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_6, pipeline_config_6, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup, NULL, config, VMEM_CONF_PIPELINE_6, NULL);
static param_t *pipeline_configs[] = {
    &pipeline_config_1, &pipeline_config_2, &pipeline_config_3, 
    &pipeline_config_4, &pipeline_config_5, &pipeline_config_6
};

/* Define a pipeline_run parameter */
static uint8_t _pipeline_run = 0;
void callback_run(param_t *param, int index);
PARAM_DEFINE_STATIC_RAM(PARAMID_PIPELINE_RUN, pipeline_run, PARAM_TYPE_UINT8, -1, 0, PM_CONF, callback_run, NULL, &_pipeline_run, "Set the pipeline to execute the file");