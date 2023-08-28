#ifndef PRISMA_BACKEND_BUFFERS_INDEXBUFFER_H
#define PRISMA_BACKEND_BUFFERS_INDEXBUFFER_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

#include "prisma/backend/buffers/buffer.h"

struct prisma_backend_device;
struct prisma_backend_commandpool;

struct prisma_backend_buffers_indexbuffer_info
{
    uint32_t index_count;
    uint32_t index_size;
    void *data;
};

struct prisma_backend_buffers_indexbuffer
{
    struct prisma_backend_buffer buffer;
    uint32_t index_count;
};

enum prisma_error prisma_backend_buffers_indexbuffer_init(
    struct prisma_backend_buffers_indexbuffer *indexbuffer, 
    struct prisma_backend_device *device,
    struct prisma_backend_commandpool *commandpool,
    struct prisma_backend_buffers_indexbuffer_info *info);

void prisma_backend_buffers_indexbuffer_destroy(struct prisma_backend_buffers_indexbuffer *indexbuffer, struct prisma_backend_device* device);

#endif /* PRISMA_BACKEND_BUFFERS_INDEXBUFFER_H */