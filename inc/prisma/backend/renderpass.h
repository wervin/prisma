#ifndef PRISMA_BACKEND_RENDERPASS_H
#define PRISMA_BACKEND_RENDERPASS_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;

struct prisma_backend_renderpass_info
{
  VkSampleCountFlagBits sample_count;
  VkFormat format;
};

struct prisma_backend_renderpass
{
  VkRenderPass vk_renderpass;
};

enum prisma_error prisma_backend_renderpass_init(struct prisma_backend_renderpass *renderpass,
                                                 struct prisma_backend_device *device,
                                                 struct prisma_backend_renderpass_info *info);

void prisma_backend_renderpass_destroy(struct prisma_backend_renderpass *renderpass,
                                       struct prisma_backend_device *device);

#endif /* PRISMA_BACKEND_RENDERPASS_H */