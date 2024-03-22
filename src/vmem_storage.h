#pragma once
#include <vmem/vmem.h>
extern vmem_t vmem_storage;

#define VMEM_CONF_PIPELINE_1 0x00   // 188 bytes to next address
#define VMEM_CONF_PIPELINE_2 0xBC   // 188 bytes apart from previous address
#define VMEM_CONF_PIPELINE_3 0x178  // 188 bytes apart from previous address
#define VMEM_CONF_PIPELINE_4 0x234  // 188 bytes apart from previous address
#define VMEM_CONF_PIPELINE_5 0x2F0  // 188 bytes apart from previous address
#define VMEM_CONF_PIPELINE_6 0x3AC  // 188 bytes apart from previous address

#define VMEM_CONF_MODULE_1 0x468    // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_2 0x524    // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_3 0x5E0    // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_4 0x69C    // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_5 0x758    // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_6 0x814    // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_7 0x8D0    // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_8 0x98C    // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_9 0xA48    // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_10 0xB04   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_11 0xBC0   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_12 0xC7C   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_13 0xD38   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_14 0xDF4   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_15 0xEB0   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_16 0xF6C   // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_17 0x1028  // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_18 0x10E4  // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_19 0x11A0  // 188 bytes apart from previous address
#define VMEM_CONF_MODULE_20 0x125C  // 188 bytes apart from previous address

#define VMEM_ERROR_CODE 0x1318