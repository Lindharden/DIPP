#ifndef DIPP_UPLOAD_H
#define DIPP_UPLOAD_H

#define UPLOAD_TIMEOUT 10000
#define VMEM_LOCATE_TIMEOUT 10000
#define RADIO_NODE_ID 150
#define VMEM_NAME "image_buffer"

/**
 * Upload data to another node at the specified address.
 * @param node Target node for upload.
 * @param address Virtual address at target node.
 * @param data Image batch data to upload to radio. 
 * @param len Length of data to upload.
*/
void upload(int node, int address, char *data, int len);

#endif