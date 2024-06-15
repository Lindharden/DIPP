#include <unistd.h>
#include <vmem/vmem_client.h>
#include <vmem/vmem_server.h>
#include <param/param.h>
#include <param/param_client.h>
#include "dipp_error.h"
#include "vmem_upload.h"
#include "vmem_upload_param.h"

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

vmem_list2_t vmem_file = {0};
uint8_t buffer_node_id = 0;
void upload(unsigned char *data, int len)
{
    uint8_t node = param_get_uint8(&radio_node_id);
    int timeout = 1000;

    if (vmem_file.vaddr == 0 || node != buffer_node_id)
    {
        if (vmem_client_find(node, VMEM_LOCATE_TIMEOUT, &vmem_file, 2, VMEM_NAME, 5) < 0)
        {   
            set_error_param(INTERNAL_VMEM_NOT_FOUND);
            return;
        }
        buffer_node_id = node;
    }

    if (vmem_file.vaddr == 0)
    {
        buffer_node_id = 0;
        set_error_param(INTERNAL_VMEM_NOT_FOUND);
    }

	uint32_t _head;
	uint32_t _tail;
	uint32_t _list[BUFFER_LIST_SIZE];
	PARAM_DEFINE_REMOTE_DYNAMIC(PARAMID_BUFFER_LIST, buffer_list, node, PARAM_TYPE_UINT32, BUFFER_LIST_SIZE, sizeof(uint32_t), PM_REMOTE, &_list, NULL);
	PARAM_DEFINE_REMOTE_DYNAMIC(PARAMID_BUFFER_HEAD, buffer_head, node, PARAM_TYPE_UINT32, -1, 0, PM_REMOTE, &_head, NULL);
	PARAM_DEFINE_REMOTE_DYNAMIC(PARAMID_BUFFER_TAIL, buffer_tail, node, PARAM_TYPE_UINT32, -1, 0, PM_REMOTE, &_tail, NULL);
    
    /* Add remote parameters to local parameters */
    param_list_add(&buffer_head);
    param_list_add(&buffer_tail);
    param_list_add(&buffer_list);
    
    if (param_pull_single(&buffer_head, -1, 1, 0, node, timeout, 2) < 0)
    {
        set_error_param(INTERNAL_REMOTE_PARAM_PULL);
        return;
    }
    if (param_pull_single(&buffer_tail, -1, 1, 0, node, timeout, 2) < 0)
    {
        set_error_param(INTERNAL_REMOTE_PARAM_PULL);
        return;        
    }
    if (param_pull_single(&buffer_list, -1, 1, 0, node, timeout, 2) < 0)
    {
        set_error_param(INTERNAL_REMOTE_PARAM_PULL);
        return;        
    }

    uint32_t head = _head;
    uint32_t tail = _tail;
    uint32_t head_offset = _list[head];
    uint32_t tail_offset = _list[tail];

    uint32_t insert_offset = head_offset + len > BUFFER_VMEM_SIZE ? 0 : head_offset;
    uint32_t new_head_offset = insert_offset + len;

    if (overtake(head_offset, tail_offset, new_head_offset)) 
    {
        uint32_t next_tail = next(tail);
        uint32_t next_tail_offset = _list[next_tail];

        while (!overtake(tail_offset, new_head_offset, next_tail_offset))
        {
            tail_offset = next_tail_offset;
            next_tail = next(next_tail);
            next_tail_offset = _list[next_tail];
        }
        
        tail = next_tail;
    }

    uint32_t new_head = next(head);
    uint32_t new_tail = tail == new_head ? next(tail) : tail;

    if (param_push_single(&buffer_head, -1, &new_head, 0, node, timeout, 2, 1) < 0)
    {
        set_error_param(INTERNAL_REMOTE_PARAM_PUSH);
        return;
    }
    if (param_push_single(&buffer_tail, -1, &new_tail, 0, node, timeout, 2, 1) < 0)
    {
        set_error_param(INTERNAL_REMOTE_PARAM_PUSH);
        return;
    }
    if (param_push_single(&buffer_list, new_head, &new_head_offset, 0, node, timeout, 2, 1) < 0)
    {
        set_error_param(INTERNAL_REMOTE_PARAM_PUSH);
        return;
    }
    if (vmem_upload(node, timeout, vmem_file.vaddr + insert_offset, (char *)data, len, 2) < 0)
    {
        set_error_param(INTERNAL_VMEM_UPLOAD);
        return;
    }
}