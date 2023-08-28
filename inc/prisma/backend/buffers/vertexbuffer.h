#ifndef PRISMA_BACKEND_BUFFERS_VERTEXBUFFER_H
#define PRISMA_BACKEND_BUFFERS_VERTEXBUFFER_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

#include "prisma/backend/buffers/buffer.h"

struct prisma_backend_device;
struct prisma_backend_commandpool;

struct prisma_backend_buffers_vertexbuffer_info
{
    uint32_t vertex_count;
    uint32_t vertex_size;
    void *data;
};

struct prisma_backend_buffers_vertexbuffer
{
    struct prisma_backend_buffer buffer;
};

enum prisma_error prisma_backend_buffers_vertexbuffer_init(
    struct prisma_backend_buffers_vertexbuffer *vertexbuffer, 
    struct prisma_backend_device *device,
    struct prisma_backend_commandpool *commandpool,
    struct prisma_backend_buffers_vertexbuffer_info *info);

void prisma_backend_buffers_vertexbuffer_destroy(struct prisma_backend_buffers_vertexbuffer *vertexbuffer, struct prisma_backend_device* device);

#endif /* PRISMA_BACKEND_BUFFERS_VERTEXBUFFER_H */