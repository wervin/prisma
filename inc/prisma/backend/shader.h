#ifndef PRISMA_BACKEND_SHADER_H
#define PRISMA_BACKEND_SHADER_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_device;

struct prisma_backend_shader_info
{
    const char *path;
};

struct prisma_backend_shader
{
    VkShaderModule vk_shader_module;
};

enum prisma_error prisma_backend_shader_init(struct prisma_backend_shader *shader, struct prisma_backend_device *device, struct prisma_backend_shader_info *info);
void prisma_backend_shader_destroy(struct prisma_backend_shader *shader, struct prisma_backend_device *device);

#endif /* PRISMA_BACKEND_SHADER_H */