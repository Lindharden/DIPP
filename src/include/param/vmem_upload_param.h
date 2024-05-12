#ifndef VMEM_UPLOAD_PARAM_H
#define VMEM_UPLOAD_PARAM_H

#include <param/param.h>
#include "vmem_storage.h"
#include "dipp_paramids.h"

/* Define CSP parameter for radio node ID */
PARAM_DEFINE_STATIC_VMEM(PARAMID_RADIO_NODE_ID, radio_node_id, PARAM_TYPE_UINT8, -1, 0, PM_CONF, NULL, NULL, storage, VMEM_RADIO_NODE_ID, "Radio CSP node ID");

#endif