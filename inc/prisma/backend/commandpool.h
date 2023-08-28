#ifndef PRISMA_BACKEND_COMMAND_POOL_H
#define PRISMA_BACKEND_COMMAND_POOL_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;

struct prisma_backend_commandpool
{
  VkCommandPool vk_commandpool;
};

enum prisma_error prisma_backend_commandpool_init(
    struct prisma_backend_commandpool *commandpool,
    struct prisma_backend_device *device);

void prisma_backend_commandpool_destroy(
    struct prisma_backend_commandpool *commandpool,
    struct prisma_backend_device *device);

#endif /* PRISMA_BACKEND_COMMAND_POOL_H */