#ifndef VMEM_UPLOAD_PARAM_H
#define VMEM_UPLOAD_PARAM_H

#include <param/param.h>
#include "vmem_storage.h"
#include "dipp_paramids.h"

#define RADIO_IMAGE_BUFFER_HEAD_ID 60
#define RADIO_IMAGE_BUFFER_TAIL_ID 61

/* Define CSP parameter for radio node ID */
PARAM_DEFINE_STATIC_VMEM(PARAMID_RADIO_NODE_ID, radio_node_id, PARAM_TYPE_UINT8, -1, 0, PM_CONF, NULL, NULL, storage, VMEM_RADIO_NODE_ID, "Radio CSP node ID");
PARAM_DEFINE_STATIC_VMEM(PARAMID_BUFFER_LIST, blist, PARAM_TYPE_UINT32, 10, sizeof(uint32_t), PM_CONF, NULL, NULL, storage, VMEM_BUFFER_LIST, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_BUFFER_HEAD, bhead, PARAM_TYPE_UINT32, -1, 0, PM_CONF, NULL, NULL, storage, VMEM_BUFFER_HEAD, NULL);
PARAM_DEFINE_STATIC_VMEM(PARAMID_BUFFER_TAIL, btail, PARAM_TYPE_UINT32, -1, 0, PM_CONF, NULL, NULL, storage, VMEM_BUFFER_TAIL, NULL);

#endif