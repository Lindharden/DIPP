#pragma once
#include <vmem/vmem.h>
extern vmem_t vmem_config;

#define VMEM_CONF_BOOTCNT_ADDR 0x00 //2 bytes
#define VMEM_CONF_RUNFILE_ADDR 0x02 //200 bytes
#define VMEM_CONF_RUNONBOOT_ADDR 0xCA // (=202) 1 byte
#define VMEM_CONF_SLOTNAME_ADDR 0xCB // (=203) 50 bytes
#define VMEM_CONF_ARB_PARAM_INT_0_ADDR 0xFD // (=253) 2 bytes
#define VMEM_CONF_ARB_PARAM_INT_1_ADDR 0xFF // (=255) 2 bytes
#define VMEM_CONF_ARB_PARAM_INT_2_ADDR 0x101 // (=257) 2 bytes
#define VMEM_CONF_ARB_PARAM_FLOAT_0_ADDR 0x103 // (=259) 4 bytes
#define VMEM_CONF_ARB_PARAM_FLOAT_1_ADDR 0x107 // (=263) 4 bytes
#define VMEM_CONF_DISINTEGRATOR_STATE_ADDR 0x111 // (=267) 2 bytes
