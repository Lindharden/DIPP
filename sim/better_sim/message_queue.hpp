#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <stdlib.h>
#include "common.hpp"

class MessageQueue{
    private:
        int createMemorySpaceKey();
        int createMemorySpace(size_t size, int key);
        void* insertMemory(unsigned char *data, size_t size, int shm_id);
        bool sendMessage(ImageBatch batch);

    public:
        MessageQueue();
        ~MessageQueue();
        bool SendImage(ImageBatch batch, u_int16_t* error);
};

#endif