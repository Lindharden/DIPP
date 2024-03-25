#include <unistd.h>
#include <vmem/vmem_client.h>
#include <vmem/vmem_server.h>
#include <param/param_client.h>
#include "vmem_upload.h"
#include "vmem_upload_param.h"

static vmem_list_t vmem_radio = {0};

void init_radio()
{
    /* Find address of image buffer vmem on radio */
    vmem_radio = vmem_client_find(RADIO_NODE_ID, VMEM_LOCATE_TIMEOUT, 2, VMEM_NAME, sizeof(VMEM_NAME) - 1);
}

void upload(char *data, int len)
{
    /* Fetch radio vmem if not done yet */
    if (vmem_radio.vaddr == 0)
        init_radio();

    /* Get head address of radio vmem image buffer to write image data to */
    if (param_pull_single(&vmem_upload_head, -1, 1, false, RADIO_NODE_ID, UPLOAD_TIMEOUT, 2) < 0)
        printf("Retrieving parameter value failed\n");

    uint32_t write_address = vmem_radio.vaddr + _head;

    /* Upload image file to CSP node */
    printf("  Upload %u bytes to node %u addr 0x%x\n", len, RADIO_NODE_ID, write_address);
    vmem_upload(RADIO_NODE_ID, UPLOAD_TIMEOUT, write_address, data, len, 2);

    /* Update head addres of radio vmem image buffer */
    uint32_t new_head = write_address + len;
    if (param_push_single(&vmem_upload_head, -1, &new_head, false, RADIO_NODE_ID, UPLOAD_TIMEOUT, 2, true) < 0)
        printf("Push parameter value failed\n");
    
    printf("  Waiting for flash driver to flush\n");
    usleep(100000); // ???
}