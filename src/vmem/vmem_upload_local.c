#include <unistd.h>
#include <vmem/vmem_client.h>
#include <param/param.h>
#include <param/param_client.h>
#include <vmem/vmem_ring.h>
#include "dipp_error.h"
#include "vmem_upload_local.h"
#include "vmem_upload_param.h"
#include "protobuf/metadata.pb-c.h"

/* Local ring-buffer */
void upload(unsigned char *data, int num, int len) 
{
    uint8_t node = param_get_uint8(&radio_node_id);
    int timeout = 1000;

    uint32_t offset = 0;
    int image_index = 0;
    while (image_index < num && offset < len)
    {
        uint32_t start = offset;

        uint32_t meta_size = *((uint32_t *)(data + offset));
        offset += sizeof(uint32_t); // Move the offset to the start of metadata
        Metadata *metadata = metadata__unpack(NULL, meta_size, data + offset);
        offset += meta_size; // Move offset to start of image
        offset += metadata->size; // Move the offset to the start of the next image block

        uint32_t end = offset;
        image_index++;

        vmem_ring_upload(node, timeout, "images", (char *)(data + start), end - start, 2);
    }
}