#ifndef DIPP_ERROR_H
#define DIPP_ERROR_H

#include "dipp_paramids.h"
#include "vmem_storage.h"
#include <param/param.h>
#include <stdlib.h>
#include <stdint.h>

/* Error domain codes */
typedef enum ERROR_CODE
{
    MEMORY_MALLOC = 100,
    MEMORY_REALLOC = 101,
    MEMORY_FREE = 102,

    MSGQ_NOT_FOUND = 200,
    MSGQ_EMPTY = 201,

    SHM_NOT_FOUND = 300,
    SHM_DETACH = 301,
    SHM_REMOVE = 302,
    SHM_ATTACH = 303,

    PIPE_READ = 400,
    PIPE_EMPTY = 401,

    INTERNAL_PID_NOT_FOUND = 500,
    INTERNAL_SO_NOT_FOUND = 501,
    INTERNAL_RUN_NOT_FOUND = 502,
    INTERNAL_PARAM_BOOL_NOT_FOUND = 503,
    INTERNAL_PARAM_INT_NOT_FOUND = 504,
    INTERNAL_PARAM_FLOAT_NOT_FOUND = 505,
    INTERNAL_PARAM_STRING_NOT_FOUND = 506,
    
    MODULE_EXIT_CRASH = 600,
    MODULE_EXIT_NORMAL = 601,

    MODULE_EXIT_CUSTOM = 700
} ERROR_CODE;

extern uint8_t err_current_pipeline;
extern uint8_t err_current_module;

uint32_t get_error_as_uint32(ERROR_CODE code);
void set_error_param(ERROR_CODE error_code);

/* Define error log parameters */
PARAM_DEFINE_STATIC_VMEM(PARAMID_ERROR_LOG, log_status, PARAM_TYPE_UINT32, -1, 0, PM_CONF, NULL, NULL, storage, VMEM_ERROR_CODE, "Latest error code");

#endif