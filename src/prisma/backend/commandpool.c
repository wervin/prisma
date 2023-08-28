#include <stdlib.h>

#include "prisma/backend/commandpool.h"
#include "prisma/backend/device.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_commandpool_init(struct prisma_backend_commandpool *commandpool, struct prisma_backend_device *device)
{
    VkCommandPoolCreateInfo commandpool_info = {0};
    commandpool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandpool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandpool_info.queueFamilyIndex = device->vk_graphic_queue_index;
    if (vkCreateCommandPool(device->vk_device, &commandpool_info, NULL, &commandpool->vk_commandpool) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create command pool");
        return PRISMA_ERROR_VK;
    }

    return PRISMA_ERROR_NONE;
}

void prisma_backend_commandpool_destroy(struct prisma_backend_commandpool *commandpool, struct prisma_backend_device *device)
{
    vkDestroyCommandPool(device->vk_device, commandpool->vk_commandpool, NULL);
}
