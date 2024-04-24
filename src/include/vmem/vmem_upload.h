#ifndef DIPP_UPLOAD_H
#define DIPP_UPLOAD_H

#define UPLOAD_TIMEOUT 10000
#define VMEM_LOCATE_TIMEOUT 10000
#define VMEM_NAME "image_buffer.vmem"

#include <vmem/vmem_file.h>
#include <vmem/vmem.h>
extern vmem_t vmem_buffer;
#define BUFFER_LIST_SIZE 10
#define BUFFER_VMEM_SIZE 1000000

/**
 * Upload data to another node at the specified address.
 * @param data Image batch data to upload to radio. 
 * @param len Length of data to upload.
*/
void upload_remote(unsigned char *data, int len);
void upload_local(unsigned char *data, int len);

#endif