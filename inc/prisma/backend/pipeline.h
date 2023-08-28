#ifndef PRISMA_BACKEND_PIPELINE_H
#define PRISMA_BACKEND_PIPELINE_H

#include <vulkan/vulkan.h>

#include "prisma/error.h"

struct prisma_backend_window;
struct prisma_backend_device;
struct prisma_backend_shader;
struct prisma_backend_pipelinelayout;
struct prisma_backend_renderpass;

struct prisma_backend_pipeline
{
    VkPipeline vk_pipeline;
};

enum prisma_error prisma_backend_pipeline_init(struct prisma_backend_pipeline *pipeline,
                                            struct prisma_backend_device *device,
                                            struct prisma_backend_renderpass *renderpass,
                                            struct prisma_backend_pipelinelayout *pipelinelayout,
                                            struct prisma_backend_shader *vertex_shader,
                                            struct prisma_backend_shader *frag_shader);
void prisma_backend_pipeline_destroy(struct prisma_backend_pipeline *pipeline, struct prisma_backend_device *device);

#endif /* PRISMA_BACKEND_PIPELINE_H */