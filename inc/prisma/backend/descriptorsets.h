#ifndef PRISMA_BACKEND_DESCRIPTORSETS_H
#define PRISMA_BACKEND_DESCRIPTORSETS_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;
struct prisma_backend_descriptorsetlayout;
struct prisma_backend_descriptorpool;
struct prisma_backend_buffers_uniformbuffer;

struct prisma_backend_descriptorsets_info
{
  uint32_t max_frames_in_flight;
};

struct prisma_backend_descriptorsets
{
  VkDescriptorSet *vk_descriptorsets;
  uint32_t max_frames_in_flight;
};

enum prisma_error prisma_backend_descriptorsets_init(
    struct prisma_backend_descriptorsets *descriptorsets,
    struct prisma_backend_device *device,
    struct prisma_backend_descriptorpool *descriptorpool,
    struct prisma_backend_descriptorsetlayout *descriptorsetlayout,
    struct prisma_backend_buffers_uniformbuffer *uniformbuffer,
    struct prisma_backend_descriptorsets_info *info);

void prisma_backend_descriptorsets_destroy(struct prisma_backend_descriptorsets *descriptorsets);

#endif /* PRISMA_BACKEND_DESCRIPTORSETS_H */