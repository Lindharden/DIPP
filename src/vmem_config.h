#pragma once
#include <vmem/vmem.h>
extern vmem_t vmem_config;
extern vmem_t vmem_proto;

#define VMEM_CONF_CONFIG 0x00 //200 bytes
#define VMEM_CONF_PROTO 0xC8 //200 bytes
