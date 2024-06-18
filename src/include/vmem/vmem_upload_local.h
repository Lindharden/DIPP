#ifndef DIPP_UPLOAD_H
#define DIPP_UPLOAD_H

#define UPLOAD_TIMEOUT 10000
#define VMEM_LOCATE_TIMEOUT 10000

#include <vmem/vmem_ring.h>

/**
 * Upload data to another node at the specified address.
 * @param data Image batch data to upload to radio. 
 * @param num Amount of images to upload.
 * @param len Length of data to upload.
*/
void upload(unsigned char *data, int num, int len);


#endif