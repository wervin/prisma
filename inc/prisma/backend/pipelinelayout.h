#ifndef PRISMA_BACKEND_PIPELINELAYOUT_H
#define PRISMA_BACKEND_PIPELINELAYOUT_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;
struct prisma_backend_descriptorsetlayout;

struct prisma_backend_pipelinelayout
{
    VkPipelineLayout vk_pipelinelayout;
};

enum prisma_error prisma_backend_pipelinelayout_init(
    struct prisma_backend_pipelinelayout *pipelinelayout, 
    struct prisma_backend_device *device,
    struct prisma_backend_descriptorsetlayout *descriptorsetlayout);

void prisma_backend_pipelinelayout_destroy(struct prisma_backend_pipelinelayout *pipelinelayout, struct prisma_backend_device *device);

#endif /* PRISMA_BACKEND_PIPELINELAYOUT_H */