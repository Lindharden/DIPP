

/* Error domain codes */
typedef enum
{
    PRELOADING = 1,
    DATA_RECEPTION = 2,
    PROCESSING = 3,
    OFFLOADING = 4
} ERROR_DOMAIN;

/* Preloading codes */
typedef enum
{
    SETUP_MODULE = 1,
    SETUP_PIPELINE = 2,
    SETUP_LOAD_PARAM = 3,
} PRELOADING_DOMAIN;

/* Module codes */
typedef enum
{
    MODULE_FIND_SO_FILE = 1,
    MODULE_MEMORY_ALLOCATION = 2,
} MODULE_DOMAIN;

/* Find SO File codes */
typedef enum
{
    FILE_NOT_FOUND = 1,
    RUN_NOT_FOUND = 2,
} FIND_SO_FILE_DOMAIN;

/* Memory Allocation codes */
typedef enum
{
    MODULE_PARAMETER_LIST = 1,
    MODULE_PARAMETER = 2,
} MEMORY_ALLOCATION_DOMAIN;

/* Pipeline codes */
typedef enum
{
    GET_PARAMETER_BY_ID = 1,
} PIPELINE_DOMAIN;

/* Load Param codes */
typedef enum
{
    LOAD_PARAM_MEMORY_ALLOCATION = 1,
} LOAD_PARAM_DOMAIN;







/* Data Reception codes */
typedef enum
{
    MSG_QUEUE = 1,
    SHARED_MEMORY = 2,
} DATA_RECEPTION_DOMAIN;

/* Msg Queue codes */
typedef enum
{
    GET_MSG_QUEUE = 1,
    MSG_QUEUE_RECEIVE = 2,
} MSG_QUEUE_DOMAIN;

/* Shared Memory codes */
typedef enum
{
    SHARED_MEMORY_GET = 1,
} SHARED_MEMORY_DOMAIN;