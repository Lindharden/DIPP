#ifndef DIPP_CONFIGS_H
#define DIPP_CONFIGS_H

#include <stdlib.h>
#include <stdint.h>

#define DATA_PARAM_SIZE 188

#define MAX_MODULES 20
#define MAX_PIPELINES 6

#define PIPELINE_PARAMID_OFFSET 10
#define MODULE_PARAMID_OFFSET 30

/* Structs for storing module and pipeline configurations */
typedef struct Module {
    char *module_name;
    void *module_function;
    int module_param_id;
} Module;

typedef struct Pipeline {
    int pipeline_id;
    Module modules[MAX_MODULES];
    int num_modules;
} Pipeline;

/* Local structures for saving module parameter configurations (translated from Protobuf) */
typedef enum 
{
    NOT_SET = 0,
    BOOL_VALUE = 2,
    INT_VALUE = 3,
    FLOAT_VALUE = 4,
    STRING_VALUE = 5
} ModuleParameter__ValueCase;

typedef struct ModuleParameter
{
    char *key;
    ModuleParameter__ValueCase value_case;
    union {
        int bool_value;
        int int_value;
        float float_value;
        char *string_value;
    };
} ModuleParameter;

typedef struct ModuleParameterList
{
    size_t n_parameters;
    ModuleParameter **parameters;
} ModuleParameterList;

/* Stashed pipelines and module parameters */
extern Pipeline pipelines[];
extern ModuleParameterList module_parameter_lists[];

/* Preload all configurations if not done yet */
void setup_cache_if_needed();

#endif