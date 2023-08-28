#ifndef PRISMA_BACKEND_SYNC_H
#define PRISMA_BACKEND_SYNC_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;

struct prisma_backend_sync_info
{
    uint32_t max_frames_in_flight;
};

struct prisma_backend_sync
{
    VkFence *vk_render_fence;
    VkSemaphore *vk_present_semaphore;
    VkSemaphore *vk_render_semaphore;
    uint32_t max_frames_in_flight;
};

enum prisma_error prisma_backend_sync_init(
    struct prisma_backend_sync *sync,
    struct prisma_backend_device *device,
    struct prisma_backend_sync_info *info);

void prisma_backend_sync_destroy(
    struct prisma_backend_sync *sync,
    struct prisma_backend_device *device);

#endif  /* PRISMA_BACKEND_SYNC_H */