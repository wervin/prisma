#ifndef PRISMA_BACKEND_COMMANDBUFFERS_H
#define PRISMA_BACKEND_COMMANDBUFFERS_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;
struct prisma_backend_commandpool;
struct prisma_engine_info;

struct prisma_backend_commandbuffers_info
{
  uint32_t max_frames_in_flight;
};

struct prisma_backend_commandbuffers
{
  VkCommandBuffer *vk_commandbuffers;
  uint32_t max_frames_in_flight;
};

enum prisma_error prisma_backend_commandbuffers_init(
    struct prisma_backend_commandbuffers *commandbuffers,
    struct prisma_backend_device *device,
    struct prisma_backend_commandpool *commandpool,
    struct prisma_backend_commandbuffers_info *info);

void prisma_backend_commandbuffers_destroy(
    struct prisma_backend_commandbuffers *commandbuffers,
    struct prisma_backend_device *device,
    struct prisma_backend_commandpool *commandpool);

#endif /* PRISMA_BACKEND_COMMANDBUFFERS_H */