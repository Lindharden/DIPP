#include <unistd.h>
#include <vmem/vmem_client.h>
#include <vmem/vmem_server.h>
#include <param/param.h>
#include <param/param_client.h>
#include "dipp_error.h"
#include "vmem_upload.h"
#include "vmem_upload_param.h"

VMEM_DEFINE_FILE(buffer, "buffer", "buffer.vmem", 1000000);

static int overtake(int old, int target, int new) 
{
    if (old < target && target < new) return 1; // Simple overtake
    if (target < old && target < new && new < old) return 1; // Wraparound overtake
    return 0;
}

static int next(int current) 
{ 
    return (current + 1) % BUFFER_LIST_SIZE;
}

void upload_local(unsigned char *data, int len)
{
    uint32_t head = param_get_uint32(&bhead);
    uint32_t tail = param_get_uint32(&btail);
    uint32_t head_offset = param_get_uint32_array(&blist, head);
    uint32_t tail_offset = param_get_uint32_array(&blist, tail);

    uint32_t insert_offset = head_offset + len > BUFFER_VMEM_SIZE ? 0 : head_offset;
    uint32_t new_head_offset = insert_offset + len;

    if (overtake(head_offset, tail_offset, new_head_offset)) 
    {
        uint32_t next_tail = next(tail);
        uint32_t next_tail_offset = param_get_int32_array(&blist, next_tail);

        while (!overtake(tail_offset, new_head_offset, next_tail_offset))
        {
            tail_offset = next_tail_offset;
            next_tail = next(next_tail);
            next_tail_offset = param_get_int32_array(&blist, next_tail);
        }
        
        tail = next_tail;
    }

    uint32_t new_head = next(head);
    uint32_t new_tail = tail == new_head ? next(tail) : tail;
    param_set_uint32(&bhead, new_head);
    param_set_uint32(&btail, new_tail);
    param_set_uint32_array(&blist, new_head, new_head_offset);

    vmem_file_write(&vmem_buffer, insert_offset, (char *)data, len);
}

vmem_list2_t vmem_radio = {0};
uint8_t current_radio_node_id = 0;

void upload_remote(unsigned char *data, int len)
{
    /* Fetch radio CSP node ID */
    uint8_t radio_id = param_get_uint8(&radio_node_id);

    /* Find image_buffer vmem file on radio */
    if (vmem_radio.vaddr == 0 || radio_id != current_radio_node_id)
    {
        if (vmem_client_find(radio_id, VMEM_LOCATE_TIMEOUT, &vmem_radio, 2, VMEM_NAME, 5) < 0)
        {   
            // error when locating the vmem file
            set_error_param(INTERNAL_VMEM_NOT_FOUND);
            return;
        }
        // remove old parameter, as the id might have changed
        if (current_radio_node_id)
            param_list_remove(current_radio_node_id, 1);
        current_radio_node_id = radio_id;
    }

    /* Set up remote param for radio image buffer head */
    uint64_t _head = 0;
    PARAM_DEFINE_REMOTE_DYNAMIC(RADIO_IMAGE_BUFFER_HEAD_ID, vmem_upload_head, current_radio_node_id, PARAM_TYPE_UINT64, -1, 1, PM_REMOTE, &_head, NULL);

    /* Add head parameter to local parameters */
    param_list_add(&vmem_upload_head);

    if (vmem_radio.vaddr == 0)
    {
        // VMEM radio setup failed
        current_radio_node_id = 0;
        set_error_param(INTERNAL_VMEM_NOT_FOUND);
    }

    /* Get head address of radio vmem image buffer to write image data to */
    if (param_pull_single(&vmem_upload_head, -1, 1, 0, vmem_upload_head.node, UPLOAD_TIMEOUT, 2) < 0)
    {
        set_error_param(INTERNAL_REMOTE_PARAM_PULL);
        return;
    }

    uint64_t write_address = vmem_radio.vaddr + _head;

    /* Upload image file to CSP node */
    printf("Upload %u bytes to node %u addr 0x%lx\n", len, current_radio_node_id, write_address);
    if (vmem_upload(current_radio_node_id, UPLOAD_TIMEOUT, write_address, (char *)data, len, 2) < 0)
    {
        set_error_param(INTERNAL_VMEM_UPLOAD);
        return;
    }

    /* Update head addres of radio vmem image buffer */
    uint64_t new_head = _head + len;
    if (param_push_single(&vmem_upload_head, -1, &new_head, 1, vmem_upload_head.node, UPLOAD_TIMEOUT, 2, true) < 0)
    {
        set_error_param(INTERNAL_REMOTE_PARAM_PUSH);
        return;
    }
}