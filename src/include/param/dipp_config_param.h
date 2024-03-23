#include <param/param.h>
#include "dipp_paramids.h"
#include "vmem_storage.h"

#define DATA_PARAM_SIZE 188

/* Define module configuration parameters */
void setup_module_config(param_t *param, int index);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_1,  module_param_1,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_1,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_2,  module_param_2,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_2,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_3,  module_param_3,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_3,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_4,  module_param_4,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_4,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_5,  module_param_5,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_5,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_6,  module_param_6,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_6,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_7,  module_param_7,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_7,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_8,  module_param_8,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_8,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_9,  module_param_9,  PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_9,  NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_10, module_param_10, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_10, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_11, module_param_11, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_11, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_12, module_param_12, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_12, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_13, module_param_13, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_13, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_14, module_param_14, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_14, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_15, module_param_15, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_15, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_16, module_param_16, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_16, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_17, module_param_17, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_17, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_18, module_param_18, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_18, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_19, module_param_19, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_19, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_PARAM_20, module_param_20, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_module_config, NULL, storage, VMEM_CONF_MODULE_20, NULL);

/* Define pipeline configuration parameters */
void setup_pipeline(param_t *param, int index);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_1, pipeline_config_1, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_pipeline, NULL, storage, VMEM_CONF_PIPELINE_1, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_2, pipeline_config_2, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_pipeline, NULL, storage, VMEM_CONF_PIPELINE_2, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_3, pipeline_config_3, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_pipeline, NULL, storage, VMEM_CONF_PIPELINE_3, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_4, pipeline_config_4, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_pipeline, NULL, storage, VMEM_CONF_PIPELINE_4, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_5, pipeline_config_5, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_pipeline, NULL, storage, VMEM_CONF_PIPELINE_5, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_PIPELINE_CONFIG_6, pipeline_config_6, PARAM_TYPE_DATA, DATA_PARAM_SIZE, 0, PM_CONF, setup_pipeline, NULL, storage, VMEM_CONF_PIPELINE_6, NULL);

param_t *module_config_params[] = {
    &module_param_1,  &module_param_2,  &module_param_3,  &module_param_4,  &module_param_5, 
    &module_param_6,  &module_param_7,  &module_param_8,  &module_param_9,  &module_param_10, 
    &module_param_11, &module_param_12, &module_param_13, &module_param_14, &module_param_15, 
    &module_param_16, &module_param_17, &module_param_18, &module_param_19, &module_param_20 
};

param_t *pipeline_config_params[] = {
    &pipeline_config_1, &pipeline_config_2, &pipeline_config_3, 
    &pipeline_config_4, &pipeline_config_5, &pipeline_config_6
};