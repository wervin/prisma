#include <stdlib.h>

#include "prisma/backend/commandbuffers.h"
#include "prisma/backend/commandpool.h"
#include "prisma/backend/device.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_commandbuffers_init(
    struct prisma_backend_commandbuffers *commandbuffers, 
    struct prisma_backend_device *device, 
    struct prisma_backend_commandpool *commandpool,
    struct prisma_backend_commandbuffers_info *info)
{
    commandbuffers->max_frames_in_flight = info->max_frames_in_flight;
    commandbuffers->vk_commandbuffers = malloc(commandbuffers->max_frames_in_flight * sizeof(VkCommandBuffer));
    if (commandbuffers->vk_commandbuffers == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }

    VkCommandBufferAllocateInfo commandbuffers_info = {0};
    commandbuffers_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandbuffers_info.commandPool = commandpool->vk_commandpool;
    commandbuffers_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandbuffers_info.commandBufferCount = commandbuffers->max_frames_in_flight;
    if (vkAllocateCommandBuffers(device->vk_device, &commandbuffers_info, commandbuffers->vk_commandbuffers) != VK_SUCCESS)
    {
        PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create command buffer");
        return PRISMA_ERROR_VK;
    }

    return PRISMA_ERROR_NONE;
}

void prisma_backend_commandbuffers_destroy(
    struct prisma_backend_commandbuffers *commandbuffers, 
    struct prisma_backend_device *device,
    struct prisma_backend_commandpool *commandpool)
{
    if (commandbuffers->vk_commandbuffers)
    {
        vkFreeCommandBuffers(device->vk_device, commandpool->vk_commandpool, commandbuffers->max_frames_in_flight, commandbuffers->vk_commandbuffers);
        free(commandbuffers->vk_commandbuffers);
    }
}