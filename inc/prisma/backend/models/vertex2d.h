#ifndef PRISMA_BACKEND_MODELS_VERTEX2D_H
#define PRISMA_BACKEND_MODELS_VERTEX2D_H

#include <cglm/cglm.h>

#include <vulkan/vulkan.h>

struct prisma_backend_models_vertex2d_input
{
    VkVertexInputBindingDescription binding;
    VkVertexInputAttributeDescription attributes[1];
};

struct prisma_backend_models_vertex2d
{
    vec2 position;
};

struct prisma_backend_models_vertex2d_input prisma_backend_models_vertex2d_get_input(void);

#endif /* PRISMA_BACKEND_MODELS_VERTEX2D_H */