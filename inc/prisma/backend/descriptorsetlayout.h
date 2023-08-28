#ifndef PRISMA_BACKEND_DESCRIPTORSETLAYOUT_H
#define PRISMA_BACKEND_DESCRIPTORSETLAYOUT_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;

struct prisma_backend_descriptorsetlayout
{
  VkDescriptorSetLayout vk_descriptorsetlayout;
};

enum prisma_error prisma_backend_descriptorsetlayout_init(
    struct prisma_backend_descriptorsetlayout *descriptorsetlayout,
    struct prisma_backend_device *device);

void prisma_backend_descriptorsetlayout_destroy(
    struct prisma_backend_descriptorsetlayout *descriptorsetlayout,
    struct prisma_backend_device *device);

#endif /* PRISMA_BACKEND_DESCRIPTORSETLAYOUT_H */