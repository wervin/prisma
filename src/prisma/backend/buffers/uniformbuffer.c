#include <stdlib.h>

#include "prisma/backend/buffers/uniformbuffer.h"
#include "prisma/backend/buffers/buffer.h"

#include "prisma/backend/device.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_buffers_uniformbuffer_init(
    struct prisma_backend_buffers_uniformbuffer *uniformbuffer, 
    struct prisma_backend_device *device,
    struct prisma_backend_buffers_uniformbuffer_info *info)
{
    uniformbuffer->max_frames_in_flight = info->max_frames_in_flight;
    uniformbuffer->size = info->size;

    uniformbuffer->buffers = malloc(uniformbuffer->max_frames_in_flight * sizeof(struct prisma_backend_buffer));
    if (uniformbuffer->buffers == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }

    uniformbuffer->mapped_buffers = malloc(uniformbuffer->max_frames_in_flight * sizeof(void *));
    if (uniformbuffer->mapped_buffers == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }

    struct prisma_backend_buffer_info uniformbuffer_info = {
        .size = info->size,
        .memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
    };
    
    for (uint32_t i = 0; i < uniformbuffer->max_frames_in_flight; i++)
    {
        prisma_backend_buffer_create(&uniformbuffer->buffers[i], device, &uniformbuffer_info);
        vkMapMemory(device->vk_device, uniformbuffer->buffers[i].vk_devicememory, 0, info->size, 0, &uniformbuffer->mapped_buffers[i]);
    }

    return PRISMA_ERROR_NONE;
}

void prisma_backend_buffers_uniformbuffer_destroy(
    struct prisma_backend_buffers_uniformbuffer *uniformbuffer, 
    struct prisma_backend_device* device)
{
    if (uniformbuffer->buffers)
    {
        for (uint32_t i = 0; i < uniformbuffer->max_frames_in_flight; i++)
            prisma_backend_buffer_destroy(&uniformbuffer->buffers[i], device);
        free(uniformbuffer->buffers);
    }

    if (uniformbuffer->mapped_buffers)
        free(uniformbuffer->mapped_buffers);
}
