#ifndef DIPP_ERROR_PARAM_H
#define DIPP_ERROR_PARAM_H

#include <param/param.h>
#include "dipp_paramids.h"
#include "vmem_storage.h"

/* Define error log parameter */
PARAM_DEFINE_STATIC_VMEM(PARAMID_ERROR_LOG, log_status, PARAM_TYPE_UINT32, -1, 0, PM_CONF, NULL, NULL, storage, VMEM_ERROR_CODE, "Latest error code");

#endif