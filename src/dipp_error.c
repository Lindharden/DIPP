#include <param/param.h>
#include <stdio.h>
#include "dipp_error.h"
#include "dipp_error_param.h"
#include "dipp_paramids.h"
#include "vmem_storage.h"

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
    printf("Error logged with value: %d\n", error_value);
}