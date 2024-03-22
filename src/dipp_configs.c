#include "dipp_configs.h"
#include "dipp_error.h"
#include "module_config.pb-c.h"
#include "pipeline_config.pb-c.h"
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

size_t get_param_buffer(uint8_t **out, param_t *param)
{
    // initialize buffer for module parameters
    int initial_buf_size = DATA_PARAM_SIZE;
    uint8_t buf[initial_buf_size];
    param_get_data(param, buf, initial_buf_size);
    int buf_size = (int)buf[0];

    *out = malloc(buf_size * sizeof(uint8_t));
    if (!*out)
    {
        set_error_param(MEMORY_MALLOC);
        return 0;
    }

    // Copy the data from the original buffer to the new buffer
    for (size_t i = 0; i < buf_size; i++)
    {
        (*out)[i] = buf[i + 1];
    }

    return buf_size;
}

// Function to load a module and parameter from a configuration file
void *load_module(char *moduleName)
{
    char filename[256]; // Adjust the buffer size as needed
    snprintf(filename, sizeof(filename), "./external_modules/%s.so", moduleName);

    // Load the external library dynamically
    void *handle = dlopen(filename, RTLD_LAZY);
    if (handle == NULL)
    {
        set_error_param(INTERNAL_SO_NOT_FOUND);
        return NULL;
    }

    // Get a function pointer to the external function
    void *functionPointer = dlsym(handle, "run");
    if (functionPointer == NULL)
    {
        set_error_param(INTERNAL_RUN_NOT_FOUND);
        dlclose(handle);
        return NULL;
    }

    return functionPointer;
}

void setup_pipeline(param_t *param, int index)
{
    uint8_t *buffer = NULL;
    size_t buf_size = get_param_buffer(&buffer, param);

    PipelineDefinition *pdef = pipeline_definition__unpack(NULL, buf_size, buffer);
    free(buffer);
    buffer = NULL;

    if (!pdef)
    {
        return; // Skip this pipeline if unpacking fails
    }

    int pipeline_id = param->id - PIPELINE_PARAMID_OFFSET;
    pipelines[pipeline_id].pipeline_id = pipeline_id + 1;
    pipelines[pipeline_id].num_modules = pdef->n_modules;

    for (size_t module_idx = 0; module_idx < pdef->n_modules; module_idx++)
    {
        ModuleDefinition *mdef = pdef->modules[module_idx];
        pipelines[pipeline_id].modules[module_idx].module_name = strdup(mdef->name);
        pipelines[pipeline_id].modules[module_idx].module_function = load_module(mdef->name);
        pipelines[pipeline_id].modules[module_idx].module_param_id = mdef->param_id - 1;
    }
}

void setup_module_config(param_t *param, int index)
{
    uint8_t *buffer = NULL;
    size_t buf_size = get_param_buffer(&buffer, param);

    ModuleConfig *mcon = module_config__unpack(NULL, buf_size, buffer);
    free(buffer);
    buffer = NULL; 

    if (!mcon)
    {
        return; // Skip this module if unpacking fails
    }

    int module_id = param->id - MODULE_PARAMID_OFFSET; // Minus 30 cause IDs are offset by 30 to accommodate pipeline ids (see pipeline.h)
    module_parameter_lists[module_id].n_parameters = mcon->n_parameters;
    module_parameter_lists[module_id].parameters = malloc(mcon->n_parameters * sizeof(ModuleParameter *));
    if (!module_parameter_lists[module_id].parameters) // Check if malloc failed
    {
        set_error_param(MEMORY_MALLOC);
        module_config__free_unpacked(mcon, NULL);
        return;
    }

    for (size_t i = 0; i < mcon->n_parameters; i++)
    {
        module_parameter_lists[module_id].parameters[i] = malloc(sizeof(ModuleParameter));

        if (!module_parameter_lists[module_id].parameters[i]) // Check if malloc failed
        {   
            set_error_param(MEMORY_MALLOC);
            // Cleanup previously allocated memory for parameters
            for (size_t j = 0; j < i; j++)
            {
                if (module_parameter_lists[module_id].parameters[j]->value_case == STRING_VALUE)
                {
                    free(module_parameter_lists[module_id].parameters[j]->string_value);
                }
                free(module_parameter_lists[module_id].parameters[j]);
            }
            free(module_parameter_lists[module_id].parameters);

            module_config__free_unpacked(mcon, NULL); // Assume there's a way to free mcon
            return;
        }

        module_parameter_lists[module_id].parameters[i]->key = mcon->parameters[i]->key;
        module_parameter_lists[module_id].parameters[i]->value_case = mcon->parameters[i]->value_case;

        switch (mcon->parameters[i]->value_case)
        {
            case CONFIG_PARAMETER__VALUE_BOOL_VALUE:
                module_parameter_lists[module_id].parameters[i]->bool_value = mcon->parameters[i]->bool_value;
                break;
            case CONFIG_PARAMETER__VALUE_INT_VALUE:
                module_parameter_lists[module_id].parameters[i]->int_value = mcon->parameters[i]->int_value;
                break;
            case CONFIG_PARAMETER__VALUE_FLOAT_VALUE:
                module_parameter_lists[module_id].parameters[i]->float_value = mcon->parameters[i]->float_value;
                break;
            case CONFIG_PARAMETER__VALUE_STRING_VALUE:
                module_parameter_lists[module_id].parameters[i]->string_value = mcon->parameters[i]->string_value;
                break;
            default:
                break;
        }
    }
}

void setup_all_pipelines()
{
    for (size_t pipeline_idx = 0; pipeline_idx < MAX_PIPELINES; pipeline_idx++)
    {
        setup_pipeline(pipeline_config_params[pipeline_idx], 0);
    }
}

void setup_all_module_configs()
{
    for (size_t module_idx = 0; module_idx < MAX_MODULES; module_idx++)
    {
        setup_module_config(module_config_params[module_idx], 0);
    }
}

void setup_cache_if_needed()
{
    if (!is_setup)
    {
        // Fetch and setup pipeline and module configurations if not done
        setup_all_pipelines();
        setup_all_module_configs();
        is_setup = 1;
    }
}