#include <stdlib.h>

#include "prisma/backend/sync.h"
#include "prisma/backend/device.h"

#include "prisma/log.h"

enum prisma_error prisma_backend_sync_init(
    struct prisma_backend_sync *sync,
    struct prisma_backend_device *device,
    struct prisma_backend_sync_info *info)
{
    sync->max_frames_in_flight = info->max_frames_in_flight;

    sync->vk_render_fence = malloc(sync->max_frames_in_flight * sizeof(VkFence));
    if (sync->vk_render_fence == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }

    sync->vk_render_semaphore = malloc(sync->max_frames_in_flight * sizeof(VkSemaphore));
    if (sync->vk_render_semaphore == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }

    sync->vk_present_semaphore = malloc(sync->max_frames_in_flight * sizeof(VkSemaphore));
    if (sync->vk_present_semaphore == NULL) {
        PRISMA_LOG_ERROR(PRISMA_ERROR_MEMORY, "Failed to allocate memory");
        return PRISMA_ERROR_MEMORY;
    }
    
    VkFenceCreateInfo fence_info = {0};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkSemaphoreCreateInfo semaphore_info = {0};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (uint32_t i = 0; i < sync->max_frames_in_flight; i++)
    {
        if (vkCreateFence(device->vk_device, &fence_info, NULL, &sync->vk_render_fence[i]) != VK_SUCCESS)
        {
            PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create render fence");
            return PRISMA_ERROR_VK;
        }
        if (vkCreateSemaphore(device->vk_device, &semaphore_info, NULL, &sync->vk_render_semaphore[i]) != VK_SUCCESS)
        {
            PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create render semaphore");
            return PRISMA_ERROR_VK;
        }
        if (vkCreateSemaphore(device->vk_device, &semaphore_info, NULL, &sync->vk_present_semaphore[i])  != VK_SUCCESS)
        {
            PRISMA_LOG_ERROR(PRISMA_ERROR_VK, "Failed to create present semaphore");
            return PRISMA_ERROR_VK;
        }
    }

    return PRISMA_ERROR_NONE;
}

void prisma_backend_sync_destroy(
    struct prisma_backend_sync *sync,
    struct prisma_backend_device *device)
{
    for (uint32_t i = 0; i < sync->max_frames_in_flight; i++)
    {
        vkDestroyFence(device->vk_device, sync->vk_render_fence[i], NULL);
        vkDestroySemaphore(device->vk_device, sync->vk_render_semaphore[i], NULL);
        vkDestroySemaphore(device->vk_device, sync->vk_present_semaphore[i], NULL);
    }
}