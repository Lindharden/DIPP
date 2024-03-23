#ifndef VMEM_UPLOAD_PARAM_H
#define VMEM_UPLOAD_PARAM_H

#include <param/param.h>
#include "vmem_upload.h"

#define RADIO_IMAGE_BUFFER_HEAD_ID 60
#define RADIO_IMAGE_BUFFER_TAIL_ID 61

/* Define radio CSP parameters */
static uint32_t _head = 0;
static uint32_t _tail = 0;
PARAM_DEFINE_REMOTE(vmem_upload_head, RADIO_NODE_ID, RADIO_IMAGE_BUFFER_HEAD_ID, PARAM_TYPE_UINT32, -1, 0, PM_CONF, &_head, NULL);
PARAM_DEFINE_REMOTE(vmem_upload_tail, RADIO_NODE_ID, RADIO_IMAGE_BUFFER_TAIL_ID, PARAM_TYPE_UINT32, -1, 0, PM_CONF, &_tail, NULL);

#endif