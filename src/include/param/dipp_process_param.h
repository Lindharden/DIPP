#ifndef DIPP_PROCESS_PARAM_H
#define DIPP_PROCESS_PARAM_H

#include <param/param.h>
#include "dipp_paramids.h"
#include "vmem_storage.h"

/* Define a pipeline_run parameter */
static uint8_t _pipeline_run = 0;
void callback_run(param_t *param, int index);
PARAM_DEFINE_STATIC_RAM(PARAMID_PIPELINE_RUN, pipeline_run, PARAM_TYPE_UINT8, -1, 0, PM_CONF, callback_run, NULL, &_pipeline_run, "Set the pipeline to execute according to the value");

/* Define a module timeout parameter */
PARAM_DEFINE_STATIC_VMEM(PARAMID_MODULE_TIMEOUT, module_timeout, PARAM_TYPE_UINT32, -1, 0, PM_CONF, NULL, NULL, storage, VMEM_MODULE_TIMEOUT, "Module timeout in seconds");

#endif