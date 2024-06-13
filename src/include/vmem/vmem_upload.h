#ifndef DIPP_UPLOAD_H
#define DIPP_UPLOAD_H

#define UPLOAD_TIMEOUT 10000
#define VMEM_LOCATE_TIMEOUT 10000
#define VMEM_NAME "ring"

#include <vmem/vmem_ring.h>
#include <vmem/vmem.h>
#define BUFFER_LIST_SIZE 10
#define BUFFER_VMEM_SIZE 100000000

/**
 * Upload data to another node at the specified address.
 * @param data Image batch data to upload to radio. 
 * @param num Amount of images to upload.
 * @param len Length of data to upload.
*/
void upload(unsigned char *data, int num, int len);

VMEM_DEFINE_RING(ring, "ring", "ring.vmem", 1000000000, 100);

#endif