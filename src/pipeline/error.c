#include "error.h"

uint8_t err_current_pipeline = 0;
uint8_t err_current_module = 0;

uint32_t get_error_as_uint32(ERROR_CODE code)
{
    return code * 1000 + err_current_pipeline * 100 + err_current_module;
}