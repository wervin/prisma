#include <stdlib.h>
#include <string.h>

#include "prisma/backend/buffers/vertexbuffer.h"
#include "prisma/backend/buffers/buffer.h"
#include "prisma/backend/device.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_buffers_vertexbuffer_init(
    struct prisma_backend_buffers_vertexbuffer *vertexbuffer, 
    struct prisma_backend_device *device,
    struct prisma_backend_commandpool *commandpool,
    struct prisma_backend_buffers_vertexbuffer_info *info)
{
    enum prisma_error status = PRISMA_ERROR_NONE;

    struct prisma_backend_buffer staging_buffer = {0};
    
    struct prisma_backend_buffer_info staging_info = {
        .size = info->vertex_count * info->vertex_size,
        .memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    };

    status = prisma_backend_buffer_create(&staging_buffer, device, &staging_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    void* data;
    vkMapMemory(device->vk_device, staging_buffer.vk_devicememory, 0, info->vertex_count * info->vertex_size, 0, &data);
    memcpy(data, info->data, info->vertex_count * info->vertex_size);
    vkUnmapMemory(device->vk_device, staging_buffer.vk_devicememory);

    struct prisma_backend_buffer_info vertexbuffer_info = {
        .size = info->vertex_count * info->vertex_size,
        .memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    };

    status = prisma_backend_buffer_create(&vertexbuffer->buffer, device, &vertexbuffer_info);
    if (status != PRISMA_ERROR_NONE)
        return status;

    status = prisma_backend_buffer_copy(&staging_buffer, &vertexbuffer->buffer, info->vertex_count * info->vertex_size, device, commandpool);
    if (status != PRISMA_ERROR_NONE)
        return status;

    prisma_backend_buffer_destroy(&staging_buffer, device);

    return PRISMA_ERROR_NONE;
}

void prisma_backend_buffers_vertexbuffer_destroy(struct prisma_backend_buffers_vertexbuffer *vertexbuffer, struct prisma_backend_device* device)
{
    prisma_backend_buffer_destroy(&vertexbuffer->buffer, device);
}