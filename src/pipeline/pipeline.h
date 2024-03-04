#include "../param_config.h"
#include "../vmem_config.h"

#define DATA_PARAM_SIZE 188

// Error codes
#define SUCCESS 0
#define FAILURE -1

/* Define module specific parameters */
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_1, module_param_1, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_1, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_2, module_param_2, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_2, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_3, module_param_3, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_3, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_4, module_param_4, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_4, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_5, module_param_5, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_5, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_6, module_param_6, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_MODULE_6, NULL);
static param_t *params[] = {&module_param_1, &module_param_2, &module_param_3, &module_param_4, &module_param_5, &module_param_6};

/* Define pipeline configuration parameters */
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG, pipeline_config, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, NULL, NULL, config, VMEM_CONF_PIPELINE, NULL);

/* Define a pipeline_run parameter */
static uint8_t _pipeline_run = 0;
void callback_run(param_t *param, int index);
PARAM_DEFINE_STATIC_RAM(PARAMID_PIPELINE_RUN, pipeline_run, PARAM_TYPE_UINT8, -1, 0, PM_CONF, callback_run, NULL, &_pipeline_run, "Set the pipeline to execute the file");
