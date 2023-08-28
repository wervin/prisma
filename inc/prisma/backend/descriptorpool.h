#ifndef PRISMA_BACKEND_DESCRIPTORPOOL_H
#define PRISMA_BACKEND_DESCRIPTORPOOL_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;

struct prisma_backend_descriptorpool_info
{
  uint32_t max_frames_in_flight;
};

struct prisma_backend_descriptorpool
{
  VkDescriptorPool vk_descriptorpool;
};

enum prisma_error prisma_backend_descriptorpool_init(
    struct prisma_backend_descriptorpool *descriptorpool,
    struct prisma_backend_device *device,
    struct prisma_backend_descriptorpool_info *info);

void prisma_backend_descriptorpool_destroy(
    struct prisma_backend_descriptorpool *descriptorpool,
    struct prisma_backend_device *device);

#endif /* PRISMA_BACKEND_DESCRIPTORPOOL_H */