#ifndef PRISMA_BACKEND_BUFFERS_BUFFER_H
#define PRISMA_BACKEND_BUFFERS_BUFFER_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;
struct prisma_backend_commandpool;

struct prisma_backend_buffer_info
{
    uint32_t size;
    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags memory_properties;
};

struct prisma_backend_buffer
{
    VkBuffer vk_buffer;
    VkDeviceMemory vk_devicememory;
};

enum prisma_error prisma_backend_buffer_create(struct prisma_backend_buffer *buffer, 
    struct prisma_backend_device *device, struct prisma_backend_buffer_info *info);

enum prisma_error prisma_backend_buffer_copy(
    struct prisma_backend_buffer *source, 
    struct prisma_backend_buffer *destination, 
    uint32_t size,
    struct prisma_backend_device *device,
    struct prisma_backend_commandpool *commandpool);

void prisma_backend_buffer_destroy(struct prisma_backend_buffer *buffer, struct prisma_backend_device* device);

#endif /* PRISMA_BACKEND_BUFFERS_BUFFER_H */