#include <vmem/vmem_file.h>

//Define file to store persisten params
VMEM_DEFINE_FILE(config, "config", "config.vmem", 5000);
VMEM_DEFINE_FILE(proto, "proto", "proto.vmem", 5000);
