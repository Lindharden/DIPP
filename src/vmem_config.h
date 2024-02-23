#pragma once
#include <vmem/vmem.h>
extern vmem_t vmem_config;

#define VMEM_CONF_PIPELINE 0x00    // 188 bytes
#define VMEM_CONF_MODULE_1 0xBC    // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_2 0x178   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_3 0x244   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_4 0x310   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_5 0x3DC   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_6 0x4A8   // 188 bytes apart from previous address