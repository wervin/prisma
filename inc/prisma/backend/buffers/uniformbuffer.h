#ifndef PRISMA_BACKEND_BUFFERS_UNIFORMBUFFER_H
#define PRISMA_BACKEND_BUFFERS_UNIFORMBUFFER_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

#include "prisma/backend/buffers/buffer.h"

struct prisma_backend_device;
struct prisma_engine;

struct prisma_backend_buffers_uniformbuffer_info
{
    uint32_t size;
    uint32_t max_frames_in_flight;
};

struct prisma_backend_buffers_uniformbuffer
{
    struct prisma_backend_buffer *buffers;
    void **mapped_buffers;
    uint32_t max_frames_in_flight;
    uint32_t size;
};

enum prisma_error prisma_backend_buffers_uniformbuffer_init(
    struct prisma_backend_buffers_uniformbuffer *uniformbuffer, 
    struct prisma_backend_device *device,
    struct prisma_backend_buffers_uniformbuffer_info *info);

void prisma_backend_buffers_uniformbuffer_destroy(
    struct prisma_backend_buffers_uniformbuffer *uniformbuffer, 
    struct prisma_backend_device* device);

#endif /* PRISMA_BACKEND_BUFFERS_UNIFORMBUFFER_H */