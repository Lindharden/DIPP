#pragma once
#include <vmem/vmem.h>
extern vmem_t vmem_config;

#define VMEM_CONF_BOOTCNT_ADDR 0x00 //2 bytes
#define VMEM_CONF_RUNFILE_ADDR 0x02 //200 bytes
#define VMEM_CONF_RUNONBOOT_ADDR 0xCA // (=202) 1 byte
#define VMEM_CONF_SLOTNAME_ADDR 0xCB // (=203) 50 bytes
