#include <unistd.h>
#include <vmem/vmem_client.h>
#include <vmem/vmem_server.h>
#include <param/param.h>
#include <param/param_client.h>
#include "vmem_upload.h"
#include "vmem_upload_param.h"

static vmem_list_t vmem_radio = {0};

// PARAM_DEFINE_REMOTE_DYNAMIC(RADIO_IMAGE_BUFFER_HEAD_ID, vmem_upload_head, RADIO_NODE_ID, PARAM_TYPE_UINT32, -1, 1, PM_CONF, &_head, NULL);
PARAM_DEFINE_REMOTE(vmem_upload_head, RADIO_NODE_ID, RADIO_IMAGE_BUFFER_HEAD_ID, PARAM_TYPE_UINT32, -1, 0, PM_CONF, &_head, NULL);

void init_radio()
{
    /* Find address of image buffer vmem on radio */
    vmem_radio = vmem_client_find(RADIO_NODE_ID, VMEM_LOCATE_TIMEOUT, 1, VMEM_NAME, strlen(VMEM_NAME));
}

void upload(char *data, int len)
{
    /* Fetch radio vmem if not done yet */
    if (vmem_radio.vaddr == 0)
        init_radio();

    /* Get head address of radio vmem image buffer to write image data to */
    if (param_pull_single(&vmem_upload_head, -1, 1, 1, vmem_upload_head.node, UPLOAD_TIMEOUT, 2) < 0)
        printf("Retrieving parameter value failed\n");     

    uint32_t write_address = vmem_radio.vaddr + _head;

    /* Upload image file to CSP node */
    printf("  Upload %u bytes to node %u addr 0x%x\n", len, RADIO_NODE_ID, write_address);
    vmem_upload(RADIO_NODE_ID, UPLOAD_TIMEOUT, write_address, data, len, 2);

    /* Update head addres of radio vmem image buffer */
    uint32_t new_head = write_address + len;
    if (param_push_single(&vmem_upload_head, -1, &new_head, 1, vmem_upload_head.node, UPLOAD_TIMEOUT, 2, true) < 0)
        printf("Push parameter value failed\n");
    
    printf("  Waiting for flash driver to flush\n");
    usleep(100000); // ???
}