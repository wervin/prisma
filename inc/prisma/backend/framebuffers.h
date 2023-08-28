#ifndef PRISMA_BACKEND_FRAMEBUFFERS_H
#define PRISMA_BACKEND_FRAMEBUFFERS_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;
struct prisma_backend_swapchain;
struct prisma_backend_renderpass;

struct prisma_backend_framebuffers
{
  VkFramebuffer* vk_framebuffers;
  uint32_t framebuffer_count;
};

enum prisma_error prisma_backend_framebuffers_init(
    struct prisma_backend_framebuffers *framebuffers,
    struct prisma_backend_device *device,
    struct prisma_backend_swapchain *swapchain,
    struct prisma_backend_renderpass *renderpass);

void prisma_backend_framebuffers_destroy(
    struct prisma_backend_framebuffers *framebuffers,
    struct prisma_backend_device *device);

#endif /* PRISMA_BACKEND_FRAMEBUFFERS_H */