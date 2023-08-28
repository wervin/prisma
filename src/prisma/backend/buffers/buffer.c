#include "prisma/backend/buffers/buffer.h"

#include "prisma/backend/device.h"
#include "prisma/backend/commandpool.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_buffer_create(struct prisma_backend_buffer *buffer, 
    struct prisma_backend_device *device, struct prisma_backend_buffer_info *info)
{
    VkBufferCreateInfo buffer_info = {0};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = info->size;
    buffer_info.usage = info->usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(device->vk_device, &buffer_info, NULL, &buffer->vk_buffer) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create vertex buffer");
        return PRISMA_ERROR_VK;
    }

    VkMemoryRequirements mem_requirements = {0};
    vkGetBufferMemoryRequirements(device->vk_device, buffer->vk_buffer, &mem_requirements);

    VkPhysicalDeviceMemoryProperties mem_poperties = {0};
    vkGetPhysicalDeviceMemoryProperties(device->vk_physical_device, &mem_poperties);

    uint32_t memory_type_index = 0;
    while (
        (memory_type_index < mem_poperties.memoryTypeCount) &&
        !((mem_requirements.memoryTypeBits & (1 << memory_type_index)) &&
          ((mem_poperties.memoryTypes[memory_type_index].propertyFlags & info->memory_properties) == info->memory_properties)))
    {
        memory_type_index++;
    }

    VkMemoryAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = memory_type_index;
    if (vkAllocateMemory(device->vk_device, &alloc_info, NULL, &buffer->vk_devicememory) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to allocate vertex buffer memory");
        return PRISMA_ERROR_VK;
    }

    vkBindBufferMemory(device->vk_device, buffer->vk_buffer, buffer->vk_devicememory, 0);

    return PRISMA_ERROR_NONE;
}

enum prisma_error prisma_backend_buffer_copy(
    struct prisma_backend_buffer *source, 
    struct prisma_backend_buffer *destination, 
    uint32_t size,
    struct prisma_backend_device *device,
    struct prisma_backend_commandpool *commandpool)
{
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandpool->vk_commandpool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandbuffer;
    vkAllocateCommandBuffers(device->vk_device, &allocInfo, &commandbuffer);

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandbuffer, &beginInfo);

    VkBufferCopy copyRegion = {0};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandbuffer, source->vk_buffer, destination->vk_buffer, 1, &copyRegion);

    vkEndCommandBuffer(commandbuffer);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandbuffer;

    vkQueueSubmit(device->vk_graphic_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->vk_graphic_queue);
    vkFreeCommandBuffers(device->vk_device, commandpool->vk_commandpool, 1, &commandbuffer);

    return PRISMA_ERROR_NONE;
}

void prisma_backend_buffer_destroy(struct prisma_backend_buffer *buffer, struct prisma_backend_device* device)
{
    vkDestroyBuffer(device->vk_device, buffer->vk_buffer, NULL);
    vkFreeMemory(device->vk_device, buffer->vk_devicememory, NULL);
}