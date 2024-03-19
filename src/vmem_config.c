#include <vmem/vmem_file.h>

/* Define file to store persistent params */
VMEM_DEFINE_FILE(config, "config", "config.vmem", 10000);

/* Define file to store persistent error logs */
VMEM_DEFINE_FILE(log, "log", "log.vmem", 50);