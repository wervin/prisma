#include "prisma/backend/descriptorpool.h"
#include "prisma/backend/device.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_descriptorpool_init(
    struct prisma_backend_descriptorpool *descriptorpool,
    struct prisma_backend_device *device,
    struct prisma_backend_descriptorpool_info *info)
{
    VkDescriptorPoolSize pool_size = {0};
    pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size.descriptorCount = info->max_frames_in_flight * 2;
    
    VkDescriptorPoolCreateInfo pool_info = {0};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.maxSets = pool_size.descriptorCount;

    if (vkCreateDescriptorPool(device->vk_device, &pool_info, NULL, &descriptorpool->vk_descriptorpool) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create descriptor pool");
        return PRISMA_ERROR_VK;
    }

    return PRISMA_ERROR_NONE;
}

void prisma_backend_descriptorpool_destroy(
    struct prisma_backend_descriptorpool *descriptorpool,
    struct prisma_backend_device *device)
{
    vkDestroyDescriptorPool(device->vk_device, descriptorpool->vk_descriptorpool, NULL);
}