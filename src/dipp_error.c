#include "dipp_error.h"

/* Define error log parameters */
PARAM_DEFINE_STATIC_VMEM(PARAMID_ERROR_LOG, log_status, PARAM_TYPE_UINT32, -1, 0, PM_CONF, NULL, NULL, storage, VMEM_ERROR_CODE, "Latest error code");

uint8_t err_current_pipeline = 0;
uint8_t err_current_module = 0;

uint32_t get_error_as_uint32(ERROR_CODE code)
{
    return code * 1000 + err_current_pipeline * 100 + err_current_module;
}

void set_error_param(ERROR_CODE error_code)
{
    uint32_t error_value = get_error_as_uint32(error_code);
    param_set_uint32(&log_status, error_value);
}